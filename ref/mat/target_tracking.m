function [range, velocity] = target_tracking(difference_matrix, r_min_index, v_min_index, r, vel, diff_energy_line)

%{
Function Name: target_tracking
Description: Target tracking
Input:
	difference_matrix: RD matrix difference
	r_min_index: Min range index
	v_min_index: Min velocity index
	r: Beat frequency correspiratoryonding to range axis
	vel: Doppler frequency correspirationonding to velocity axis
	diff_energy_line: Background difference energy line
Output: None
Return:
	range: Target range
	velocity: Target velocity
%}

% Target detection
[~, maxInd_velocity] = max(max(difference_matrix, [], 1)); % Target velocity
[maxVal_range, maxInd_range] = max(max(difference_matrix, [], 2)); % Target range
if maxVal_range > diff_energy_line(maxInd_range, 1)
	range = r(r_min_index+ maxInd_range- 1);
	velocity = vel(v_min_index+ maxInd_velocity- 1);
else
	range = 0;
	velocity = 0;
end

end