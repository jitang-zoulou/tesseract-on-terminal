#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>
#include <algorithm>

struct Vec4 { float x, y, z, w; };
struct Vec3 { float x, y, z; };
struct Edge { int a, b, color; };

// 表现深度
const std::string palette = " .-~:;=!*#$@";

// 12种ANSI颜色
const std::string ansi_colors[12] = {
    "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m", // 基础6色
    "\033[91m", "\033[92m", "\033[93m", "\033[94m", "\033[95m", "\033[96m"  // 高亮6色
};
const std::string ansi_reset = "\033[0m";

// 获取终端动态宽高
void getTerminalSize(int& width, int& height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    width = w.ws_col;
    height = w.ws_row;
}

// 找两点决定的维度
int log2_exact(int x) {
    if (x == 1) return 0; if (x == 2) return 1;
    if (x == 4) return 2; if (x == 8) return 3;
    return -1;
}

// 从边数组中寻找特定端点的边索引
int get_edge_idx(const std::vector<Edge>& edges, int u, int v) {
    for (int i = 0; i < 32; ++i) {
        if ((edges[i].a == u && edges[i].b == v) || (edges[i].a == v && edges[i].b == u)) return i;
    }
    return -1;
}

// 带z-buffer和独立颜色的bresenham
void drawLineColor(int x0, int y0, float z0, int x1, int y1, float z1, int color_code,
                   std::vector<char>& buffer, std::vector<int>& color_buffer, 
                   std::vector<float>& zbuffer, int width, int height) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    int steps = std::max(std::abs(x1 - x0), std::abs(y1 - y0));
    float z_step = steps > 0 ? (z1 - z0) / steps : 0;
    float curr_z = z0;

    while (true) {
        // 各线程操作自己的局部buffer
        if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height) {
            int idx = y0 * width + x0;
            if (curr_z > zbuffer[idx]) {
                zbuffer[idx] = curr_z;
                float norm_z = std::clamp((curr_z + 2.0f) / 4.0f, 0.0f, 1.0f);
                int char_idx = static_cast<int>(norm_z * (palette.length() - 1));
                buffer[idx] = palette[char_idx];
                color_buffer[idx] = color_code;
            }
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
        curr_z += z_step;
    }
}

int main() {
    std::vector<Vec4> vertices(16);
    std::vector<Edge> edges;

    for (int i = 0; i < 16; ++i) {
        vertices[i] = { (i & 1) ? 1.0f : -1.0f, (i & 2) ? 1.0f : -1.0f, 
                        (i & 4) ? 1.0f : -1.0f, (i & 8) ? 1.0f : -1.0f };
    }

    // 拓扑连边
    for (int i = 0; i < 16; ++i) {
        for (int j = i + 1; j < 16; ++j) {
            int diff = i ^ j;
            if (diff == 1 || diff == 2 || diff == 4 || diff == 8) {
                edges.push_back({i, j, -1});
            }
        }
    }

    // 面约束边缘着色算法
    int color_start = 0;
    for (int e = 0; e < 32; ++e) {
        int u = edges[e].a, v = edges[e].b;
        int dim1 = log2_exact(u ^ v);
        bool used[12] = {false};

        // 查找与边 e 共享二维面的所有其它边
        for (int dim2 = 0; dim2 < 4; ++dim2) {
            if (dim1 == dim2) continue;
            int v3 = u ^ (1 << dim2), v4 = v ^ (1 << dim2);
            int e1 = get_edge_idx(edges, u, v3);
            int e2 = get_edge_idx(edges, v, v4);
            int e3 = get_edge_idx(edges, v3, v4);

            // 标记同一面的边为已污染
            if (e1 != -1 && edges[e1].color != -1) used[edges[e1].color] = true;
            if (e2 != -1 && edges[e2].color != -1) used[edges[e2].color] = true;
            if (e3 != -1 && edges[e3].color != -1) used[edges[e3].color] = true;
        }

        // 轮询分配12种颜色
        for (int offset = 0; offset < 12; ++offset) {
            int c = (color_start + offset) % 12;
            if (!used[c]) {
                edges[e].color = c;
                color_start = (c + 1) % 12; 
                break;
            }
        }
    }

    // 多核线程池配置
    unsigned int num_cores = std::thread::hardware_concurrency();
    int num_threads = (num_cores > 0) ? std::min(32, static_cast<int>(num_cores)) : 4;

    float angle_XW = 0.0f, angle_YZ = 0.0f, angle_XY = 0.0f;
    const float speed_YZ = 0.60f, speed_XW = 0.75f, speed_XY = 0.35f;

    std::cout << "\033[?25l"; 
    auto last_time = std::chrono::high_resolution_clock::now();
    const std::chrono::microseconds frame_budget(8333); // 120FPS

    // 内存池静态分配
    int last_w = 0, last_h = 0;
    std::vector<std::vector<char>> t_chars(num_threads);
    std::vector<std::vector<int>> t_colors(num_threads);
    std::vector<std::vector<float>> t_z(num_threads);

    while (true) {
        auto frame_start = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = frame_start - last_time;
        last_time = frame_start;
        float dt = std::min(elapsed.count(), 0.1f);

        // delta-time 时空积分
        angle_XW += speed_XW * dt;
        angle_YZ += speed_YZ * dt;
        angle_XY += speed_XY * dt;

        int width, height;
        getTerminalSize(width, height);

        // 扩容或清空内存池
        if (width != last_w || height != last_h) {
            for(int i=0; i<num_threads; ++i) {
                t_chars[i].assign(width * height, ' ');
                t_colors[i].assign(width * height, -1);
                t_z[i].assign(width * height, -100.0f);
            }
            last_w = width; last_h = height;
        } else {
            for(int i=0; i<num_threads; ++i) {
                std::fill(t_chars[i].begin(), t_chars[i].end(), ' ');
                std::fill(t_colors[i].begin(), t_colors[i].end(), -1);
                std::fill(t_z[i].begin(), t_z[i].end(), -100.0f);
            }
        }

        float sin_xw = std::sin(angle_XW), cos_xw = std::cos(angle_XW);
        float sin_yz = std::sin(angle_YZ), cos_yz = std::cos(angle_YZ);
        float sin_xy = std::sin(angle_XY), cos_xy = std::cos(angle_XY);

        std::vector<Vec3> proj3d(16);
        for (int i = 0; i < 16; ++i) {
            Vec4 v = vertices[i];
            float x1 = v.x * cos_xw - v.w * sin_xw, w1 = v.x * sin_xw + v.w * cos_xw;
            float y1 = v.y * cos_yz - v.z * sin_yz, z1 = v.y * sin_yz + v.z * cos_yz;
            float x2 = x1 * cos_xy - y1 * sin_xy, y2 = x1 * sin_xy + y1 * cos_xy;
            float w_scale = 1.0f / (2.5f - w1);
            proj3d[i] = { x2 * w_scale, y2 * w_scale, z1 * w_scale };
        }

        struct Point2D { int x, y; float z; };
        std::vector<Point2D> screen_pts(16);
        for (int i = 0; i < 16; ++i) {
            float z_scale = 1.0f / (2.5f - proj3d[i].z);
            screen_pts[i].x = static_cast<int>(width / 2.0f + proj3d[i].x * z_scale * height * 1.5f);
            screen_pts[i].y = static_cast<int>(height / 2.0f + proj3d[i].y * z_scale * height * 0.75f);
            screen_pts[i].z = proj3d[i].z; 
        }

        // sort-last 渲染分发
        auto worker = [&](int t_id) {
            int edges_per_thread = 32 / num_threads;
            int remainder = 32 % num_threads;
            int start = t_id * edges_per_thread + std::min(t_id, remainder);
            int end = start + edges_per_thread + (t_id < remainder ? 1 : 0);

            for (int i = start; i < end; ++i) {
                Point2D p1 = screen_pts[edges[i].a], p2 = screen_pts[edges[i].b];
                drawLineColor(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, edges[i].color,
                              t_chars[t_id], t_colors[t_id], t_z[t_id], width, height);
            }
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; ++i) threads.emplace_back(worker, i);
        for (auto& t : threads) t.join(); // 同步点

        // 并行规约
        std::string frame_output = "\033[H";
        int current_color = -2; // 状态机记录器

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int idx = y * width + x;
                float max_z = -100.0f;
                char best_char = ' ';
                int best_color = -1;

                // 从多核子buffer中提取最靠前像素
                for (int t = 0; t < num_threads; ++t) {
                    if (t_z[t][idx] > max_z) {
                        max_z = t_z[t][idx];
                        best_char = t_chars[t][idx];
                        best_color = t_colors[t][idx];
                    }
                }

                // 输出字符串
                if (best_char == ' ') {
                    frame_output += ' ';
                } else {
                    if (best_color != current_color) {
                        frame_output += ansi_colors[best_color];
                        current_color = best_color;
                    }
                    frame_output += best_char;
                }
            }
            if (y < height - 1) frame_output += "\n";
        }
        frame_output += ansi_reset; 
        std::cout << frame_output << std::flush;

        // 自旋锁
        auto work_duration = std::chrono::duration_cast<std::chrono::microseconds>(
                             std::chrono::high_resolution_clock::now() - frame_start);
        if (work_duration < frame_budget) {
            auto sleep_target = frame_budget - work_duration;
            if (sleep_target > std::chrono::microseconds(1500)) 
                std::this_thread::sleep_for(sleep_target - std::chrono::microseconds(1500));
            while (std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::high_resolution_clock::now() - frame_start) < frame_budget);
        }
    }
    return 0;
}
