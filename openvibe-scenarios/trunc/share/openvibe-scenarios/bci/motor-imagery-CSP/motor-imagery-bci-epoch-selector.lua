dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")

g_offset = nil
g_duration = nil

-- this function is called when the box is initialized
function initialize(box)
	g_offset = box:get_setting(2)
	g_duration = box:get_setting(3)
end

-- this function is called when the box is uninitialized
function uninitialize(box)
       
end

function wait_until(box, time)
  while box:get_current_time() < time do
    box:sleep()
  end
end


function wait_for(box, duration)
  wait_until(box, box:get_current_time() + duration)
end



function process(box)
	-- loops on every received stimulation for a given input
	while true do
		for stimulation = 1, box:get_stimulation_count(1) do

				-- gets the received stimulation
				identifier, date, duration = box:get_stimulation(1, 1)
				-- discards it
				box:remove_stimulation(1, 1)
				
				-- delay the OVTK_GDF_Left and Right
				if identifier == OVTK_GDF_Left or identifier == OVTK_GDF_Right then
					box:send_stimulation(1, OVTK_GDF_Correct, date+g_offset, 0)
					box:send_stimulation(1, OVTK_GDF_Incorrect, date+g_offset+g_duration, 0)
				end
		end
		box:sleep()
	end
end
