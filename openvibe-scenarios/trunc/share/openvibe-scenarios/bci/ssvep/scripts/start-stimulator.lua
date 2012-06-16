delay = 1

function initialize(box)
	s_delay = box:get_setting(2);

	io.write(string.format("Delay : [%s]\n", s_delay))

	if (s_delay:find("^%d+$") ~= nil) then
		delay = tonumber(s_delay)
	else
		io.write("[ERROR] The parameter should be a numeric value\n")
	end

end

function uninitialize(box)
end

function process(box)
	box:send_stimulation(1, 0x00008001, delay, 0)
end
