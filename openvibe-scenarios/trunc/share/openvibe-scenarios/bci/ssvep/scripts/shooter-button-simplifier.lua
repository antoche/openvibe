buffer = {}

function initialize(box)
end

function uninitialize(box)
end

-- This script receives stimulations and lets pass only every other one
-- for each stimulation code
-- Useful to remove "key up" events of keyboard stimulator
function process(box)
	
	while (true) do

		for index = 1, box:get_stimulation_count(1) do

			stimulation = box:get_stimulation(1, index)
			
			if buffer[stimulation] == nil then
				box:send_stimulation(1, stimulation, box:get_current_time(), 0)
				buffer[stimulation] = 1
			else
				buffer[stimulation] = nil
			end

			box:remove_stimulation(1, 1)
		end

		box:sleep()
	end
	


end
