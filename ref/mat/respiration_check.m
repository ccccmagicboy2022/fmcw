function [background_line] = respiration_check(data, respiration_fft_num, fs_respiration, background_line)

%{
Function Name: respiration_detection
Description: Respiration detection
Input:
	data: 1dfft data after coherent integration
	respiration_fft_num: Number of respiration fft points
	fs_respiration: Respiration equivalent sampling rate
	background_line: Background energy line
Output: None
Return:
	background_line: Background energy line
%}

% Space definition
v_fft = zeros(size(data, 1), respiration_fft_num);
lowfreq_amp = zeros(size(data, 1), 1);
% 2dfft & Remove zero frequency
for i1 = 1: size(data, 1)
    v_fft(i1, :) = fftshift(fft(data(i1, :).* hamming(size(data, 2)).', respiration_fft_num))/ respiration_fft_num;
end
v_fft(:, respiration_fft_num/ 2: respiration_fft_num/ 2+ 2) = 0;
% Checking
freq_limit_1 = ceil([0.1 0.6]/ (fs_respiration/ respiration_fft_num))+ respiration_fft_num/ 2;
freq_limit_2 = ceil([-0.6 -0.1]/ (fs_respiration/ respiration_fft_num))+ respiration_fft_num/ 2;
for i2 = 1: size(v_fft, 1)
	lowfreq_amp(i2, 1) = max(sum(abs(v_fft(i2, freq_limit_1(1, 1): freq_limit_1(1, 2)))), sum(abs(v_fft(i2, freq_limit_2(1, 1)+ 1: freq_limit_2(1, 2)))));
end
background_line = max(lowfreq_amp, background_line);

end