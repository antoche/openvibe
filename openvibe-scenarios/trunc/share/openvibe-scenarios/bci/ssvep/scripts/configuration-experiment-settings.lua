dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")

stimulation_frequencies = {}
frequency_count = 0

target_light_color = {}
target_dark_color = {}
training_target_size = {}
training_targets_positions = {}

processing_epoch_duration = nil
processing_epoch_interval = nil
processing_frequency_tolerance = nil

function initialize(box)

	for value in box:get_setting(2):gmatch("%d+") do
		table.insert(target_light_color, value)
	end

	for value in box:get_setting(3):gmatch("%d+") do
		table.insert(target_dark_color, value)
	end

	for value in box:get_setting(4):gmatch("%d+[.]?%d*") do
		table.insert(stimulation_frequencies, value)
		frequency_count = frequency_count + 1
	end

	processing_epoch_duration = box:get_setting(5)
	processing_epoch_interval = box:get_setting(6)
	processing_frequency_tolerance = box:get_setting(7)

end

function uninitialize(box)
end

function process(box)

	while box:get_stimulation_count(1) == 0 do
		box:sleep()
	end

	box:log("Info", box:get_config("Writing additional configuration to '${CustomConfigurationPrefix${OperatingSystem}}-ssvep-demo${CustomConfigurationSuffix${OperatingSystem}}'"))

	cfg_file = io.open(box:get_config("${CustomConfigurationPrefix${OperatingSystem}}-ssvep-demo${CustomConfigurationSuffix${OperatingSystem}}"), "a")

	cfg_file:write("SSVEP_TargetLightColourRed = ", target_light_color[1] / 100, "\n")
	cfg_file:write("SSVEP_TargetLightColourGreen = ", target_light_color[2] / 100, "\n")
	cfg_file:write("SSVEP_TargetLightColourBlue = ", target_light_color[3] / 100, "\n")

	cfg_file:write("SSVEP_TargetDarkColourRed = ", target_dark_color[1] / 100, "\n")
	cfg_file:write("SSVEP_TargetDarkColourGreen = ", target_dark_color[2] / 100, "\n")
	cfg_file:write("SSVEP_TargetDarkColourBlue = ", target_dark_color[3] / 100, "\n")

	for i=1,frequency_count do
		cfg_file:write("SSVEP_Frequency_", i, " = ", string.format("%g", stimulation_frequencies[i]), "\n")
	end

	cfg_file:close()

	-- create configuration files for temporal filters

	for i=1,frequency_count do
		box:log("Info", string.format("Writing file '../share/openvibe-scenarios/bci/ssvep/configuration/temporal-filter-freq-%d.cfg'", i))

		cfg_file = io.open(string.format("../share/openvibe-scenarios/bci/ssvep/configuration/temporal-filter-freq-%d.cfg", i), "w")

		cfg_file:write("<OpenViBE-SettingsOverride>\n")
		cfg_file:write("<SettingValue>Butterworth</SettingValue>\n")
		cfg_file:write("<SettingValue>Band pass</SettingValue>\n")
		cfg_file:write("<SettingValue>4</SettingValue>\n")
		cfg_file:write(string.format("<SettingValue>%g</SettingValue>\n", stimulation_frequencies[i] - processing_frequency_tolerance))
		cfg_file:write(string.format("<SettingValue>%g</SettingValue>\n", stimulation_frequencies[i] + processing_frequency_tolerance))
		cfg_file:write("<SettingValue>0.500000</SettingValue>\n")
		cfg_file:write("</OpenViBE-SettingsOverride>\n")

		cfg_file:close()
	end

	-- create configuration file for time based epoching
	
	box:log("Info", "Writing file '../share/openvibe-scenarios/bci/ssvep/configuration/time-based-epoching.cfg'")

	cfg_file = io.open("../share/openvibe-scenarios/bci/ssvep/configuration/time-based-epoching.cfg", "w")
	cfg_file:write("<OpenViBE-SettingsOverride>\n")
	cfg_file:write(string.format("<SettingValue>%g</SettingValue>\n", processing_epoch_duration))
	cfg_file:write(string.format("<SettingValue>%g</SettingValue>\n", processing_epoch_interval))
	cfg_file:write("</OpenViBE-SettingsOverride>\n")
	cfg_file:close()

	-- notify the scenario that the configuration process is complete
	
	box:send_stimulation(1, OVTK_StimulationId_TrainCompleted, box:get_current_time() + 0.2, 0)

end
