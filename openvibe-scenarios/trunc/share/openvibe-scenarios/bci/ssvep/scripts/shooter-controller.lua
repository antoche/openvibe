dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")
sequence = {}
flickeringDelay = nil

function initialize(box)
	s_sequence = box:get_setting(2)
	angular_speed = box:get_setting(3)

	box:log("Info", string.format("Target Sequence : [%s]\n", s_sequence))

	for target in s_sequence:gmatch("%d+") do
		table.insert(sequence, target)
	end

	box:log("Info", string.format("Angular Speed : %g\n", angular_speed))
	
	-- create the configuration file for the shooter application
	cfg_file = io.open(box:get_config("${CustomConfigurationPrefix${OperatingSystem}}-ssvep-demo-shooter${CustomConfigurationSuffix${OperatingSystem}}"), "w")

	cfg_file:write("# This file was automatically generated!\n\n")
	cfg_file:write("# If you want to change the SSVEP online shooter configuration\n")
	cfg_file:write("# please use the box settings in the shooter scenario.\n\n")

	cfg_file:write("SSVEP_ShipAngularSpeed = ", angular_speed, "\n")
	cfg_file:close()

end

function uninitialize(box)
end

function process(box)

	-- while targets are available, send a stimulation with new target
	-- position for each request

	while true do
		if box:get_stimulation_count(1) > 0 then

			identifier, date, duration = box:get_stimulation(1, 1)

			if identifier == OVTK_StimulationId_Target then
				current_time = box:get_current_time() + 1

				-- if no targets are available, quit the game
				if # sequence == 0 then
					box:send_stimulation(1, OVTK_StimulationId_ExperimentStop, current_time, 0)
					break
				end

				next_target = sequence[1]
				table.remove(sequence, 1)

				box:send_stimulation(1, OVTK_StimulationId_Label_00 + next_target , current_time, 0)
			end

			box:remove_stimulation(1, 1)
		end

		box:sleep()
	end

end

