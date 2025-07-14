#ifndef FILTER_MANAGER_HPP_
#define FILTER_MANAGER_HPP_

#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>

namespace load_cell {

class FilterManager {
public:
    void setRawInput(const std::vector<double>& input);

    void applyLPF();
    void applyAvg_LPF();
    void applyMedian_LPF();

    const std::vector<double>& getFilteredValues() const;

private:
    std::vector<double> raw_data;
    std::vector<double> filtered_data;

    std::vector<std::deque<double>> avg_buffers = std::vector<std::deque<double>>(8);
    double median_buffer[8][5] = {{0.0}};
    std::vector<double> lpf_state = std::vector<double>(8, 0.0);

    double applyAvg(int index, double input);
    double applyMedian(int index, double input);
    double applyLPF(int index, double input);
};

} // namespace load_cell
#endif
