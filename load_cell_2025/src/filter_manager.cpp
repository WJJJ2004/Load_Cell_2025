#include "../include/load_cell_2025/filter_manager.hpp"

namespace load_cell {

void FilterManager::setRawInput(const std::vector<double>& input) 
{
    raw_data = input;
    if (filtered_data.size() != input.size()) 
    {
        filtered_data.resize(input.size());
    }
}

void FilterManager::applyLPF() 
{
    for (size_t i = 0; i < raw_data.size(); ++i) 
    {
        filtered_data[i] = applyLPF(i, raw_data[i]);
    }
}

void FilterManager::applyAvg_LPF() 
{
    for (size_t i = 0; i < raw_data.size(); ++i) 
    {
        double avg_val = applyAvg(i, raw_data[i]);
        filtered_data[i] = applyLPF(i, avg_val);
    }
}

void FilterManager::applyMedian_LPF() 
{
    for (size_t i = 0; i < raw_data.size(); ++i) 
    {
        double med_val = applyMedian(i, raw_data[i]);
        filtered_data[i] = applyLPF(i, med_val);
    }
}

const std::vector<double>& FilterManager::getFilteredValues() const 
{
    return filtered_data;
}

double FilterManager::applyAvg(int index, double input) 
{
    auto& buf = avg_buffers[index];
    if (buf.size() >= 5) buf.pop_front();
    buf.push_back(input);

    double sum = 0.0;
    for (double val : buf) sum += val;
    return sum / static_cast<double>(buf.size());
}

double FilterManager::applyMedian(int index, double input) 
{
    for (int j = 0; j < 4; ++j) 
    {
        median_buffer[index][j] = median_buffer[index][j + 1];
    }
    median_buffer[index][4] = input;

    std::vector<double> temp(median_buffer[index], median_buffer[index] + 5);
    std::nth_element(temp.begin(), temp.begin() + 2, temp.end());
    return temp[2];
}

double FilterManager::applyLPF(int index, double input) 
{
    double sampling_time = 0.01;
    double cutoff_freq = 5.0;
    double lambda = 2.0 * M_PI * cutoff_freq * sampling_time;

    lpf_state[index] = (lambda / (1.0 + lambda)) * input + (1.0 / (1.0 + lambda)) * lpf_state[index];
    return lpf_state[index];
}

} // namespace load_cell
