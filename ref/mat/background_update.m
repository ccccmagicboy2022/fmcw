function [difference_matrix, background_data_matrix] = background_update(data, background_data_matrix, noise_coefficient, background_update_weight)

%{
Function Name: background_update
Description: Background matrix update
Input:
	data: 3d RD matrix
	background_data_matrix: 2d background data matrix
	noise_coefficient: Weighting coefficient of background noise
	background_update_weight: Background noise update weight
Output: None
Return:
	difference_matrix: RD matrix difference
	background_data_matrix: 2d background matrix
%}

data_ni = mean(data, 3); % Non-coherent integration
difference_matrix = data_ni - background_data_matrix; % Difference matrix
% Background update
background_update_index = abs(difference_matrix) < background_data_matrix* noise_coefficient; % Background matrix index to be updated
background_data_matrix(background_update_index) = background_data_matrix(background_update_index)* (1- background_update_weight) + data_ni(background_update_index)* background_update_weight; % Background matrix update

end