dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")
targets = {}
non_targets = {}
sent_stimulation = 0

function initialize(box)
	-- read the parameters of the box
	
	s_targets = box:get_setting(2)

	for t in s_targets:gmatch("%d+") do
		targets[t + 0] = true
	end

	s_non_targets = box:get_setting(3)

	for t in s_non_targets:gmatch("%d+") do
		non_targets[t + 0] = true
	end

	sent_stimulation = _G[box:get_setting(4)]

end

function uninitialize(box)
end

function process(box)

	finished = false

	while not finished do

		time = box:get_current_time()

		while box:get_stimulation_count(1) > 0 do

			s_code, s_date, s_duration = box:get_stimulation(1, 1)
			box:remove_stimulation(1, 1)
			


			if s_code >= OVTK_StimulationId_Label_00 and s_code <= OVTK_StimulationId_Label_1F then

				received_stimulation = s_code - OVTK_StimulationId_Label_00


				if targets[received_stimulation] ~= nil then
					box:send_stimulation(1, sent_stimulation, time)
				elseif non_targets[received_stimulation] ~= nil then
					box:send_stimulation(2, sent_stimulation, time)
				end

			elseif s_code == OVTK_StimulationId_ExperimentStop then
				finished = true
			end
		end

		box:sleep()

	end

end
