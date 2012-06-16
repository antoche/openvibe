dofile("../share/openvibe-plugins/stimulation/lua-stimulator-stim-codes.lua")

vrpn_host = nil
vrpn_port = nil
screen_refresh_rate = nil

function initialize(box)
	screen_refresh_rate = box:get_setting(2)
end

function uninitialize(box)
end

function process(box)

	box:log("Info", box:get_config("Generating '${CustomConfigurationPrefix${OperatingSystem}}-ssvep-demo${CustomConfigurationSuffix${OperatingSystem}}'"))

	cfg_file = io.open(box:get_config("${CustomConfigurationPrefix${OperatingSystem}}-ssvep-demo${CustomConfigurationSuffix${OperatingSystem}}"), "w")

	cfg_file:write("# This file was automatically generated!\n\n")
	cfg_file:write("# If you want to change the SSVEP configuration\n")
	cfg_file:write("# please use the provided ssvep-configuration scenario.\n")

	cfg_file:write("SSVEP_ScreenRefreshRate = ", screen_refresh_rate, "\n")

	cfg_file:close()

	box:send_stimulation(1, OVTK_StimulationId_TrainCompleted, box:get_current_time() + 0.2, 0)
end
