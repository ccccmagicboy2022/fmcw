function [range, velocity, background_data_matrix] = trajectory_tracking(data, r_min_index, v_min_index, r, vel, background_data_matrix, noise_coefficient, background_update_weight, diff_energy_line)

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
	background_update_weight: Background noise update weight
	diff_energy_line: Background difference energy line
Output: None
Return:
	range: Target range
	velocity: Target velocity
	background_data_matrix: 2d background matrix
%}

data_3dm = mean(data, 3);
difference_matrix = data_3dm - background_data_matrix; % Difference matrix
% Background update
background_update_index = abs(difference_matrix) < background_data_matrix* noise_coefficient; % Background index to be updated
background_data_matrix(background_update_index) = background_data_matrix(background_update_index)* (1- background_update_weight) + data_3dm(background_update_index)* background_update_weight; % Background matrix update
% Target detection
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