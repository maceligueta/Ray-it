#include "constants.h"
#include <vector>
#include <limits>

// EQUATIONS EXTRACTED FROM ITU-R 526-14

namespace BullingtonDiffraction {

real_number ComputeJAccordingToEq31(const real_number v) {
    if (v > -0.78) {
        return real_number(6.9 + 20 * log10(sqrt((v - 0.1)*(v - 0.1) + 1) + v - 0.1));
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

    const real_number c_e = real_number(1.0 / 8500.0);
    const size_t len = distances.size();
    const real_number d_end_to_end_distance = distances[len - 1] - distances[0];
    const real_number wave_length = SPEED_OF_LIGHT / frequency;

    for(int i=0; i<len; i++) {
        distances[i] *= real_number(0.001);
    }

    const real_number height_transmitter = heights[0];
    const real_number height_receiver = heights[len-1];

    real_number max_slope_from_transmitter = std::numeric_limits<real_number>::lowest();

    for(size_t i=1; i<len-1; i++) {
		const real_number aux = (heights[i] + real_number(500.0) * c_e * distances[i] * (d_end_to_end_distance - distances[i]) - height_transmitter) / distances[i];
        if(aux > max_slope_from_transmitter) {
            max_slope_from_transmitter = aux;
        }
	}

    const real_number slope_end_to_end = (height_receiver - height_transmitter) / d_end_to_end_distance;
    size_t point_at_which_v_is_computed = -1;

    if(max_slope_from_transmitter < slope_end_to_end) { // LOS - Direct sight
        real_number v_max = std::numeric_limits<real_number>::lowest();
        for(size_t i=1; i<len-1; i++) {
            const real_number aux = distances[i] * (d_end_to_end_distance - distances[i]);
            real_number value = heights[i] + real_number(500.0) * c_e * aux;
            value -= (height_transmitter * (d_end_to_end_distance - distances[i]) + height_receiver * distances[i]) / d_end_to_end_distance;
            value *= sqrt(real_number(0.002) * d_end_to_end_distance / (wave_length * aux));
            if(value > v_max) {
                v_max = value;
                point_at_which_v_is_computed = i;
            }
        }
        db_loss_wrt_free_space = ComputeJAccordingToEq31(v_max);
        if(db_loss_wrt_free_space > EPSILON) {
            slope_from_transmitter = (heights[point_at_which_v_is_computed]-heights[0]) / distances[point_at_which_v_is_computed];
        } else {
            slope_from_transmitter = slope_end_to_end;
        }
    }
    else { // No LOS
        real_number max_slope_to_receiver = std::numeric_limits<real_number>::lowest();
        for(size_t i=1; i<len-1; i++) {
            const real_number aux = distances[i] * (d_end_to_end_distance - distances[i]);
            real_number value = heights[i] + real_number(500.0) * c_e * aux - height_receiver;
            value /= d_end_to_end_distance - distances[i];
            if(value > max_slope_to_receiver) {
                max_slope_to_receiver = value;
            }
        }
        const real_number d_b = (height_receiver - height_transmitter + max_slope_to_receiver * d_end_to_end_distance) / (max_slope_from_transmitter + max_slope_to_receiver);
        real_number v_b = (height_transmitter + max_slope_from_transmitter * d_b - (height_transmitter * (d_end_to_end_distance - d_b) + height_receiver * d_b) / d_end_to_end_distance);
        v_b *= sqrt(real_number(0.002) * d_end_to_end_distance / (wave_length * d_b * (d_end_to_end_distance - d_b)));
        db_loss_wrt_free_space = ComputeJAccordingToEq31(v_b);
        slope_from_transmitter = max_slope_from_transmitter;
    }
}



}