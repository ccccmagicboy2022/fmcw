function [v_fft_tar_abs] = tdfft(data, r_fft_num, r_min_index, r_max_index, v_fft_num, v_min_index, v_max_index)

%{
Function Name: tdfft
Description: 2dfft
Input:
	data: Frame data
	r_fft_num: Range dimension FFT points
	r_min_index: Min range index
	r_max_index: Max range index
	v_fft_num: Doppler dimension FFT points
	v_min_index: Min velocity index
	v_max_index: Max velocity index
Output: None
Return:
	v_fft_tar_abs: Target velocity fft abs
%}

% 1dfft
r_fft = fftshift(fft(data, r_fft_num))/ r_fft_num; % Range fft
r_fft_tar = r_fft(r_min_index: r_max_index, :); % Target range fft
r_fft_tar_MF = r_fft_tar - mean(r_fft_tar, 2); % Mean filtering
% 2dfft
v_fft = fftshift(fft(r_fft_tar_MF, v_fft_num, 2), 2)/ v_fft_num; % Velocity fft
v_fft_tar = v_fft(:, v_min_index: v_max_index); % Target velocity fft
v_fft_tar_abs = abs(v_fft_tar); % Target velocity fft abs

end