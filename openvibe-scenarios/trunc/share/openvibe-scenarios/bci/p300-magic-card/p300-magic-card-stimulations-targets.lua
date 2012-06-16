#!/bin/lua

-- This Lua script generates target stimulations for the magic card demo
--
-- Author : Yann Renard, INRIA
-- Date   : 2011-03-15

dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")

-- this function is called when the box is initialized
function initialize(box)
	math.randomseed(os.time())
	card_count = box:get_setting(2)
	card_base = _G[box:get_setting(3)]
	delay = box:get_setting(4)
end

-- this function is called when the box is uninitialized
function uninitialize(box)
end

-- this function is called once by the box
function process(box)

	-- enters infinite loop
	-- cpu will be released with a call to sleep
	-- at the end of the loop
	while true do

		-- gets current simulated time
		t = box:get_current_time()

		-- loops on every received stimulation for a given input
		for stimulation = 1, box:get_stimulation_count(1) do

			-- gets stimulation
			stimulation_id, stimulation_time, stimulation_duration = box:get_stimulation(1, 1)

			if stimulation_id == OVTK_StimulationId_RestStart then

				-- triggers the target
				box:send_stimulation(1, card_base + math.random(card_count) - 1, box:get_current_time() + delay, 0)

			elseif stimulation_id == OVTK_StimulationId_ExperimentStop then

				-- triggers train stimulation
				box:send_stimulation(1, OVTK_StimulationId_Train, t+delay+1, 0)

			end

			-- discards it
			box:remove_stimulation(1, 1)
		end

		-- releases cpu
		box:sleep()
	end
end

