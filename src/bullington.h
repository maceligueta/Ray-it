#include "constants.h"
#include <vector>
#include <limits>

namespace BullingtonDiffraction {

real_number ComputeJAccordingToEq31(const real_number v) {
    if (v>-0.78) {
        return 6.9 + 20 * log10(sqrt((v - 0.1)*(v - 0.1) + 1) + v - 0.1);
    }
    else {
        return 0.0;
    }
}

void ComputeBullington(std::vector<real_number> distances,
                            const std::vector<real_number>& heights,
                            const real_number& frequency,
                            real_number& db_loss_wrt_free_space,
                            real_number& slope_from_transmitter){

    const real_number c_e = 1.0 / 8500.0;
    const size_t len = distances.size();
    const real_number d_end_to_end_distance = distances[len - 1] - distances[0];
    const real_number wave_length = SPEED_OF_LIGHT / frequency;

    for(int i=0; i<len; i++) {
        distances[i] *= 0.001;
    }

    const real_number h_ts = heights[0];
    const real_number h_rs = heights[len-1];

    real_number s_tim = std::numeric_limits<real_number>::lowest();

    for(size_t i=1; i<len-1; i++) {
		const real_number aux = (heights[i] + 500.0 * c_e * distances[i] * (d_end_to_end_distance - distances[i]) - h_ts) / distances[i];
        if(aux > s_tim) {
            s_tim = aux;
        }
	}



    const real_number slope_end_to_end = (h_rs - h_ts) / d_end_to_end_distance;

    real_number l_uc;

    if(s_tim < slope_end_to_end) {
        real_number v_max = std::numeric_limits<real_number>::lowest();
        for(size_t i=1; i<len-1; i++) {
            const real_number aux = distances[i] * (d_end_to_end_distance - distances[i]);
            real_number value = heights[i] + 500.0 * c_e * aux;
            value -= (h_ts * (d_end_to_end_distance - distances[i]) + h_rs * distances[i]) / d_end_to_end_distance;
            value *= sqrt(0.002 * d_end_to_end_distance / (wave_length * aux));
            if(value > v_max) {
                v_max = value;
            }
        }
        l_uc = ComputeJAccordingToEq31(v_max);
    }
    else {
        real_number s_rim = std::numeric_limits<real_number>::lowest();
        for(size_t i=1; i<len-1; i++) {
            const real_number aux = distances[i] * (d_end_to_end_distance - distances[i]);
            real_number value = heights[i] + 500.0 * c_e * aux - h_rs;
            value /= d_end_to_end_distance - distances[i];
            if(value > s_rim) {
                s_rim = value;
            }
        }
        const real_number d_b = (h_rs - h_ts + s_rim * d_end_to_end_distance) / (s_tim + s_rim);
        real_number v_b = (h_ts + s_tim * d_b - (h_ts * (d_end_to_end_distance - d_b) + h_rs * d_b) / d_end_to_end_distance);
        v_b *= sqrt(0.002 * d_end_to_end_distance / (wave_length * d_b * (d_end_to_end_distance - d_b)));
        l_uc = ComputeJAccordingToEq31(v_b);
    }

    slope_from_transmitter = s_tim;
    db_loss_wrt_free_space = l_uc;
}



}