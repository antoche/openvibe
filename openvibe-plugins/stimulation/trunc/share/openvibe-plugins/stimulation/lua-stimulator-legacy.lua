function initialize(box) return initialize() end
function uninitialize(box) return uninitialize() end
function process(box) return process() end
function sleep() return __openvibe_box_context:sleep() end
function get_current_time() return __openvibe_box_context:get_current_time() end
function get_stimulation_count(input_index) return __openvibe_box_context:get_stimulation_count(input_index) end
function get_stimulation(input_index, stimulation_index) return __openvibe_box_context:get_stimulation(input_index, stimulation_index) end
function remove_stimulation(input_index, stimulation_index) return __openvibe_box_context:remove_stimulation(input_index, stimulation_index) end
function send_stimulation(output_index, stimulation_identifier, stimulation_date) return __openvibe_box_context:send_stimulation(output_index, stimulation_identifier, stimulation_date) end
function send_stimulation(output_index, stimulation_identifier, stimulation_date, stimulation_duration) return __openvibe_box_context:send_stimulation(output_index, stimulation_identifier, stimulation_date, stimulation_duration) end
