close all
clearvars
clear
clc

DAQ_MODE = 2; % Data acquisition mode: 1 Simulation data / 2 Measured record data / 3 Serial port
switch DAQ_MODE
	case 1
	case 2
        [filename, pathname] = uigetfile('*.mat');
        FileName = strcat(pathname, filename);
        load(FileName)
		% Explain para for algo
        FMCWData = real(RecvSigData);
        FmcwPara.fc = FmcwDataExplain.fc* 1.0e-9; % Center frequency / GHz
        FmcwPara.BW = FmcwDataExplain.BandWidthw* 1.0e-6; % Bandwidth / MHz
        FmcwPara.PW  = FmcwDataExplain.Ramp_end_time; % Pulse width / s
        FmcwPara.PRT  = FmcwDataExplain.Chirp_cycle_time; % Pulse repetition period / s
        FmcwPara.IFFs = FmcwDataExplain.Sample_rate; % Intermediate frequency sampling rate / Hz
        FmcwPara.FrameTime = FmcwDataExplain.Frame_cycle_time; % Frame duration / s
        FmcwPara.numSamplesPerChirp = size(FMCWData, 1); % Number of fast time dimension samples
        FmcwPara.numChirpsPerFrame = size(FMCWData, 2); % Number of Chirps per Frame
        numFrames = size(FMCWData, 3); % Number of frames
	case 3
end

%% Operate para
r_fft_num = FmcwPara.numSamplesPerChirp* 2; % Range dimension FFT points / 256
r_min = 0.5; % Min range / m
r_max = 6; % Max range / m
v_fft_num = FmcwPara.numChirpsPerFrame* 4; % Doppler dimension FFT points / 64
v_min = -2.0; % Min velocity / m/s
v_max = 2.0; % Max velocity / m/s
tracking_win_time = 0.1584; % Target tracking data duration / s
tracking_step_time = 0.0792; % Target tracking data update step / s
background_win_time = 2.5344; % Background observation duration / s
respiration_win_time = 5.0688; % Respiration detection data duration / s
respiration_step_time = 2.5344; % Respiration detection data update duration / s
noise_coefficient = 1; % Weighting coefficient of background noise
background_update_weight = 0.1; % Background noise update weight
diff_energy_line = 0.5; % Background difference energy line
range_line = 4; % Range trigger line / m
background_line = 70; % Background energy line
respiration_total_times = 4; % Number of respiratory observations
respiration_effective_times = 2; % Number of effective respiratory observations

%% Const para
slope = FmcwPara.BW* 1.0e6/ FmcwPara.PW; % Slope / Hz/s
lamda = 3.0e8/ (FmcwPara.fc* 1.0e9); % Center wavelength / m
tracking_process_num = ceil(tracking_win_time/ FmcwPara.FrameTime); % Target tracking data observation times
tracking_step_process_num = ceil(tracking_step_time/ FmcwPara.FrameTime); % Target tracking data update times
background_process_num = ceil(background_win_time/ FmcwPara.FrameTime); % Background data observation times
respiration_process_num = ceil(respiration_win_time/ FmcwPara.FrameTime); % Respiration data observation times
respiration_step_process_num = ceil(respiration_step_time/ FmcwPara.FrameTime); % Respiration data update times
respiration_fft_num = respiration_process_num; % Number of respiration fft points
fs_respiration = 1.0/ FmcwPara.FrameTime; % Respiration equivalent sampling rate

%% Range para [fftN, UsefulDis]
beat_freq = linspace(-FmcwPara.IFFs/ 2.0, FmcwPara.IFFs/ 2.0- FmcwPara.IFFs/ r_fft_num, r_fft_num); % Beat frequency axis
r = 3.0e8* beat_freq./ (2* slope); % Beat frequency correspirationonding to range axis
rbin = r(1, 2)- r(1, 1); % Range interval
r_min_index = r_fft_num/ 2+ 1+ fix(r_min/ rbin); % Min range index
r_max_index = r_fft_num/ 2+ 1+ fix(r_max/ rbin); % Max range index

%% Doppler para [fftN, UsefulVel]
fs_dop = 1.0/ FmcwPara.PRT; % Doppler dimension sampling rate
dop_freq = linspace(-fs_dop/ 2.0, fs_dop/ 2.0- fs_dop/ v_fft_num, v_fft_num); % Doppler frequency axis
vel = dop_freq* lamda/ 2.0; % Doppler frequency correspirationonding to velocity axis
vbin = vel(2)- vel(1); % Velocity interval
v_min_index = v_fft_num/ 2+ 1- fix(abs(v_min)/ vbin); % Min velocity index
v_max_index = v_fft_num/ 2+ 1+ fix(v_max/ vbin); % Max velocity index

%% Index & Space & Flags
frame_index_local = 1; % Frame data acquisition counter
frame_index = 0; % Frame data counter
respiration_judge_index = 1; % Respiration judge counter
tracking_data_matrix = zeros(r_max_index- r_min_index+ 1, v_max_index- v_min_index+ 1, tracking_win_time/ FmcwPara.FrameTime); % Tracking data matrix
background_data_matrix = zeros(r_max_index- r_min_index+ 1, v_max_index- v_min_index+ 1); % Background data matrix
respiration_data_matrix = zeros(r_max_index- r_min_index+ 1, respiration_process_num); % Respiration data matrix
respiration_judge_matrix = zeros(r_max_index- r_min_index+ 1, respiration_total_times); % Respiration judge matrix
data_ready = false; % Data preparation flag
presence_flag = false; % Presence state flag
respiration_judge_flag = false; % Respiration judge flag

while(1)
	% Mode selection
	switch DAQ_MODE
		case 1
		case 2
			frame_data = FMCWData(:, :, frame_index_local);
			frame_index_local = frame_index_local + 1;
		case 3
	end
	[r_fft_tar, v_fft_tar_abs] = tdfft(frame_data, r_fft_num, r_min_index, r_max_index, v_fft_num, v_min_index, v_max_index);
	tracking_data_matrix(:, :, 1: end- 1) = tracking_data_matrix(:, :, 2: end);
	tracking_data_matrix(:, :, end) = v_fft_tar_abs;
	respiration_data_matrix(:, 1: end- 1) = respiration_data_matrix(:, 2: end);
	respiration_data_matrix(:, end) = sum(r_fft_tar, 2);
	frame_index = frame_index + 1;
	if ~data_ready
		if frame_index >= respiration_process_num- background_process_num+ 1
			background_data_matrix = background_data_matrix + v_fft_tar_abs/ ceil(background_win_time/ FmcwPara.FrameTime);
		end
		if frame_index == respiration_process_num
			data_ready = true;
			frame_index = 0;
		end
	else
		if mod(frame_index, tracking_step_process_num) == 0
			[difference_matrix, background_data_matrix] = background_update(tracking_data_matrix, background_data_matrix, noise_coefficient, background_update_weight);
			if presence_flag == false
				[range, velocity] = target_tracking(difference_matrix, r_min_index, v_min_index, r, vel, diff_energy_line);
				if range < range_line && range > 0
					presence_flag = true;
					frame_index = 0;
				end
			end
		end
		if presence_flag == true
			if frame_index == respiration_step_process_num
				[respiration_locs, respiration_state] = respiration_detection(respiration_data_matrix, respiration_fft_num, fs_respiration, background_line);
				respiration_judge_matrix(:, respiration_judge_index) = 0;
				if respiration_state
					respiration_judge_matrix(respiration_locs, respiration_judge_index) = 1;
				end
				respiration_judge_index = respiration_judge_index + 1;
				if respiration_judge_index > respiration_total_times
					respiration_judge_flag = true;
					respiration_judge_index = 1;
				end
				if respiration_judge_flag
					if max(sum(respiration_judge_matrix, 2)) >= respiration_effective_times
						presence_flag = true;
					else
						presence_flag = false;
						respiration_judge_flag = false;
						respiration_judge_index = 1;
					end
				end
				frame_index = 0;
			end
		end
	end
end