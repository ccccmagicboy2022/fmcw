function [range, velocity, background_data_matrix] = trajectory_tracking(data, r_min_index, v_min_index, r, vel, background_data_matrix, noise_coefficient, noise_update_weight, diff_energy_line)

%{
Function Name: trajectory_tracking
Description: trajectory tracking
Input:
	data: 3d data matrix
	r_min_index: Min range index
	v_min_index: Min velocity index
	r: Beat frequency correspiratoryonding to range axis
	vel: Dopper frequency correspirationonding to velocity axis
	background_data_matrix: 2d background data matrix
	noise_coefficient: Weighting coefficient of background noise
	noise_update_weight: Background noise update coefficient
	diff_energy_line: Background difference energy line
Output: None
Return:
	range: Target range
	velocity: Target velocity
	background_data_matrix: 2d background matrix
%}

data_3dm = mean(data, 3);
noise_updata_index = abs(data_3dm- background_data_matrix) < background_data_matrix* noise_coefficient; % Noise index to be updated
background_data_matrix(noise_updata_index) = background_data_matrix(noise_updata_index)* (1- noise_update_weight) + data_3dm(noise_updata_index)* noise_update_weight; % Noise vector update
% Target detection
difference_matrix = data_3dm - background_data_matrix; % Quantization matrix
[~, maxInd_velocity] = max(max(difference_matrix, [], 1)); % Target velocity
[maxVal_range, maxInd_range] = max(max(difference_matrix, [], 2)); % Target range
if maxVal_range > diff_energy_line
	range = r(r_min_index+ maxInd_range- 1);
	velocity = vel(v_min_index+ maxInd_velocity- 1);
else
	range = 0;
	velocity = 0;
end

end