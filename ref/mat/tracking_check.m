function [diff_energy_line] = tracking_check(difference_matrix, diff_energy_line)

%{
Function Name: tracking_check
Description: Self-checking
Input:
	difference_matrix: RD matrix difference
	diff_energy_line: Background difference energy line
Output: None
Return:
	diff_energy_line: Background difference energy line
%}

% Checking
diff_energy = max(difference_matrix, [], 2);
diff_energy_line = max(diff_energy, diff_energy_line);

end