function [respiration_locs, respiration_state] = respiration_detection(data, r_fft_num, respiration_fft_num, fs_respiratory, r_min_index, r_max_index, background_line)

%{
Function Name: respiration_detection
Description: respiration detection
Input:
	data: Data after coherent accumulation
	r_fft_num: Range dimension FFT points
	respiration_fft_num: Number of respiration fft points
	fs_respiratory: Respiration equivalent sampling rate
	r_min_index: Min range index
	r_max_index: Max range index
	background_line: Background energy line
Output: None
Return:
	respiration_locs: Target range index
	respiration_state: Target respiration state
%}

numFrames = size(data, 2);
Range_profile = zeros(r_fft_num, numFrames);
Doppler_profile = zeros(r_fft_num, respiration_fft_num);
lowfreq_amp = zeros(r_max_index- r_min_index+ 1, 1);
% 1dfft
for i1 = 1: numFrames
    Range_profile(:, i1) = fftshift(fft(data(:, i1), r_fft_num))/ r_fft_num;
end
% 2dfft & Remove zero frequency
for i2 = 1: r_fft_num
    Doppler_profile(i2, :) = fftshift(fft(Range_profile(i2, :), respiration_fft_num))/ respiration_fft_num;
    Doppler_profile(i2, respiration_fft_num/ 2- 1: respiration_fft_num/ 2+ 1) = 0;
end
% Respiration detection
freq_limit = ceil([0.1 3]/ (fs_respiratory/ respiration_fft_num))+ size(Doppler_profile, 2)/ 2;
for i3 = r_min_index: r_max_index
    lowfreq_amp(i3- r_min_index+ 1, 1) = sum(abs(Doppler_profile(i3, freq_limit(1, 1): freq_limit(1, 2))));
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