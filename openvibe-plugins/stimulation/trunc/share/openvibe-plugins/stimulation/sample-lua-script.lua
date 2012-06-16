dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")

-- this function is called when the box is initialized
function initialize(box)
	io.write("initialize has been called\n");

	-- inspects the box topology
	io.write(string.format("box has %i input(s)\n", box:get_input_count()))
	io.write(string.format("box has %i output(s)\n", box:get_output_count()))
	io.write(string.format("box has %i setting(s)\n", box:get_setting_count()))
	for i = 1, box:get_setting_count() do
		io.write(string.format(" - setting %i has value [%s]\n", i, box:get_setting(i)))
	end

end

-- this function is called when the box is uninitialized
function uninitialize(box)
	io.write("uninitialize has been called\n")
end

-- this function is called once by the box
function process(box)
	io.write("process has been called\n")

	-- enters infinite loop
	-- cpu will be released with a call to sleep
	-- at the end of the loop
	while true do

		-- gets current simulated time
		t = box:get_current_time()

		-- loops on all inputs of the box
		for input = 1, box:get_input_count() do

			-- loops on every received stimulation for a given input
			for stimulation = 1, box:get_stimulation_count(input) do

				-- gets the received stimulation
				identifier, date, duration = box:get_stimulation(input, 1)

				-- logs the received stimulation
				io.write(string.format("At time %f on input %i got stimulation id:%s date:%s duration:%s\n", t, input, identifier, date, duration))

				-- discards it
				box:remove_stimulation(input, 1)

				-- triggers a new OVTK_StimulationId_Label_00 stimulation five seconds after
				box:send_stimulation(1, OVTK_StimulationId_Label_00, t+5, 0)
			end
		end

		-- releases cpu
		box:sleep()
	end
end
