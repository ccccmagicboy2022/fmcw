function [respiration_locs, respiration_state] = respiration_detection(data, respiration_fft_num, fs_respiratory, background_line)

%{
Function Name: respiration_detection
Description: Respiration detection
Input:
	data: 1dfft data after coherent integration
	respiration_fft_num: Number of respiration fft points
	fs_respiratory: Respiration equivalent sampling rate
	background_line: Background energy line
Output: None
Return:
	respiration_locs: Target range index
	respiration_state: Target respiration state
%}

% Space definition
v_fft = zeros(size(data, 1), respiration_fft_num);
lowfreq_amp = zeros(size(data, 1), 1);
% 2dfft & Remove zero frequency
for i1 = 1: size(data, 1)
    v_fft(i1, :) = fftshift(fft(data(i1, :), respiration_fft_num))/ respiration_fft_num;
end
v_fft(:, respiration_fft_num/ 2: respiration_fft_num/ 2+ 2) = 0;
% Respiration detection
freq_limit = ceil([0.1 3]/ (fs_respiratory/ respiration_fft_num))+ respiration_fft_num/ 2;
for i2 = 1: size(v_fft, 1)
	lowfreq_amp(i2, 1) = sum(abs(v_fft(i2, freq_limit(1, 1): freq_limit(1, 2))));
end
target_index = find(lowfreq_amp > background_line);
if ~isempty(target_index)
	respiration_locs = target_index;
	respiration_state = true;
else
	respiration_locs = 0;
	respiration_state = false;
end

end