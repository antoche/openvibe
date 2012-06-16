dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")
sequence = {}
number_of_cycles = 0

stimulation_duration = nil
break_duration = nil
flickering_delay = nil

target_width = nil
target_height = nil

target_positions = {}
number_of_targets = {}

stimulationLabels = {
	0x00008100,
	0x00008101,
	0x00008102,
	0x00008103,
	0x00008104,
	0x00008105,
	0x00008106,
	0x00008107
}

function initialize(box)

	-- load the goal sequence
	s_sequence = box:get_setting(2)


	for target in s_sequence:gmatch("%d+") do
		table.insert(sequence, target)
		number_of_cycles = number_of_cycles + 1
	end

	box:log("Info", string.format("Number of goals in sequence: [%d]", number_of_cycles))

	-- get the duration of a stimulation sequence
	s_stimulation_duration = box:get_setting(3)

	if (s_stimulation_duration:find("^%d+[.]?%d*$") ~= nil) then
		stimulation_duration = tonumber(s_stimulation_duration)
		box:log("Info", string.format("Stimulation Duration : [%g]", stimulation_duration))
	else
		box:log("Error", "The parameter 'stimulation duration' must be a numeric value\n")
		error()
	end

	-- get the duration of a break between stimulations
	s_break_duration = box:get_setting(4)

	if (s_break_duration:find("^%d+[.]?%d*$") ~= nil) then
		break_duration = tonumber(s_break_duration)
		box:log("Info", string.format("Break Duration : [%s]", s_break_duration))
	else
		box:log("Error", "The parameter 'break duration' must be a numeric value\n")
		error()
	end

	-- get the delay between the appearance of the marker and the start of flickering
	s_flickering_delay = box:get_setting(5)

	if (s_flickering_delay:find("^%d+[.]?%d*$") ~= nil) then
		flickering_delay = tonumber(s_flickering_delay)
		box:log("Info", string.format("Flickering Delay : [%s]", s_flickering_delay))
	else
		box:log("Error", "The parameter 'flickering delay' must be a numeric value\n")
		error()
	end

	-- get the target size
	
	s_targetSize = box:get_setting(6)

	s_width, s_height = s_targetSize:match("^(%d+[.]?%d*);(%d+[.]?%d*)$")
	target_width = tonumber(s_width)
	target_height = tonumber(s_height)

	if s_width ~= nil and s_height ~= nil then
		box:log("Info", string.format("Target dimensions : width = %g, height = %g", target_width, target_height))
	else
		box:log("Error", "The parameter 'target size' must be in format float;float")
		error()
	end

	-- get the targets' positions
	
	s_targetPositions = box:get_setting(7)
	number_of_targets = 0

	for s_target_x, s_target_y in s_targetPositions:gmatch("(-?%d+[.]?%d*);(-?%d+[.]?%d*)") do
		box:log("Info", string.format("Target %d : x = %g y = %g", number_of_targets, tonumber(s_target_x), tonumber(s_target_y)))
		table.insert(target_positions, {tonumber(s_target_x), tonumber(s_target_y)})
		number_of_targets = number_of_targets + 1

	end


	-- create the configuration file for the stimulation-based-epoching
	-- this file is used during classifier training only
	cfg_file = io.open("../share/openvibe-scenarios/bci/ssvep/configuration/stimulation-based-epoching.cfg", "w")

	cfg_file:write("<OpenViBE-SettingsOverride>\n")
	cfg_file:write("	<SettingValue>", stimulation_duration, "</SettingValue>\n")
	cfg_file:write("	<SettingValue>", flickering_delay, "</SettingValue>\n")
	cfg_file:write("	<SettingValue>OVTK_StimulationId_Target</SettingValue>\n")
	cfg_file:write("</OpenViBE-SettingsOverride>\n")

	cfg_file:close()

	-- create the configuration file for the training program
	cfg_file = io.open(box:get_config("${CustomConfigurationPrefix${OperatingSystem}}-ssvep-demo-training${CustomConfigurationSuffix${OperatingSystem}}"), "w")

	cfg_file:write("# This file was automatically generated!\n\n")
	cfg_file:write("# If you want to change the SSVEP trainer configuration\n")
	cfg_file:write("# please use the box settings in the training scenario.\n\n")

	cfg_file:write("SSVEP_TargetCount = ", number_of_targets, "\n")
	cfg_file:write("SSVEP_TargetWidth = ", target_width, "\n")
	cfg_file:write("SSVEP_TargetHeight = ", target_height, "\n")

	for target_index, position in ipairs(target_positions) do

		cfg_file:write("SSVEP_Target_X_", target_index - 1, " = ", position[1], "\n")
		cfg_file:write("SSVEP_Target_Y_", target_index - 1, " = ", position[2], "\n")
	end

	cfg_file:close()

end

function uninitialize(box)
end

function process(box)

	while box:get_stimulation_count(1) == 0 do
		box:sleep()
	end

	current_time = box:get_current_time() + 1

	box:send_stimulation(1, OVTK_StimulationId_ExperimentStart, current_time, 0)

	current_time = current_time + 2

	for i,target in ipairs(sequence) do
		box:log("Info", string.format("Goal no %d is %d at %d", i, target, current_time))
		-- mark goal
		box:send_stimulation(2, OVTK_StimulationId_LabelStart + target, current_time, 0)
		-- wait for Flickering_delay seconds
		current_time = current_time + flickering_delay
		-- start flickering
		box:send_stimulation(1, OVTK_StimulationId_VisualStimulationStart, current_time, 0)
		-- wait for Stimulation_duration seconds
		current_time = current_time + stimulation_duration
		-- unmark goal and stop flickering
		box:send_stimulation(1, OVTK_StimulationId_VisualStimulationStop, current_time, 0)
		-- wait for Break_duration seconds
		current_time = current_time + break_duration
	end

	box:send_stimulation(1, OVTK_StimulationId_ExperimentStop, current_time, 0)

	box:sleep()
end

