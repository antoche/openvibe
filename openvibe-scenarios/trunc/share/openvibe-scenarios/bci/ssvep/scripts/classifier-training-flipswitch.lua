dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")

flip_count = 0
switched_flip_count = 0
flips = {}

function initialize(box)
	flip_count = box:get_input_count()

	for i = 1, flip_count do
		flips[i] = false
	end
end

function uninitialize(box)
end

function process(box)

	while switched_flip_count < flip_count do

		for i = 1, flip_count do
			if box:get_stimulation_count(i) > 0 then

				box:remove_stimulation(i, 1)

				if not flips[i] then
					switched_flip_count = switched_flip_count + 1
					flips[i] = true

					io.write("Flip ", i, " of ", flip_count, " switched\n")

				end

			end
		end

		box:sleep()
	end

	box:send_stimulation(1, OVTK_StimulationId_Label_00, box:get_current_time())
end
