function compilation
    mcc -W lib:ovm_test -T link:lib ovm_initialize.m ovm_uninitialize.m ovm_processHeader.m ovm_processBuffer.m
end