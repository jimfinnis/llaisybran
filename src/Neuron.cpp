/**
 * @file Neuron.cpp
 * @brief  Brief description of file.
 *
 */
#include "plugin.hpp"
#include "Neuron.h"
#include "utils.h"


struct Neuron : Module {
    
private:
    void configElem(int weightParam, int in) {
        configParam(weightParam,-5.0f,5.0f,0.0f,"Weight");
        configInput(in, "weight input");
    }
    
    
    inline float processInput(int weightParam, int in) {
        float inv = inputs[in].getVoltage(0);
        float w = params[weightParam].getValue();
        return inv*w;
    }
    
public:
    Neuron() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        //        printf("t,pre,sig\n");
        
        configElem(W1_PARAM, IN1_INPUT);
        configElem(W2_PARAM, IN2_INPUT);
        configElem(W3_PARAM, IN3_INPUT);
        configElem(W4_PARAM, IN4_INPUT);
        configElem(W5_PARAM, IN5_INPUT);
        
        configParam(PRESCALE_PARAM, -5.0f, 5.0f, 1.0f, "Activation scaler");
        configParam(PRESCALEMOD_PARAM, -5.0f, 5.0f, 0.0f, "Scaler modulation");
        configParam(BIAS_PARAM, -5.0f, 5.0f, 0.0f, "Bias");
        configParam(ACTFUNC_PARAM, 0.0f, 1.0f, 0.0f, "Activation function");
        configParam(POSTSCALE_PARAM, 0.0f, 10.0f, 5.0f, "Post scaling");
    }
    
    void process(const ProcessArgs& args) override {
        static int snark=0;
        float v1 = processInput(W1_PARAM, IN1_INPUT);
        float v2 = processInput(W2_PARAM, IN2_INPUT);
        float v3 = processInput(W3_PARAM, IN3_INPUT);
        float v4 = processInput(W4_PARAM, IN4_INPUT);
        float v5 = processInput(W5_PARAM, IN5_INPUT);
        
        float v = v1+v2+v3+v4+v5+params[BIAS_PARAM].getValue();
        
        v *= params[PRESCALE_PARAM].getValue() + 
              params[PRESCALEMOD_PARAM].getValue() * inputs[PRESCALEMOD_INPUT].getVoltage(0);
        
        
        float out;
        if(params[ACTFUNC_PARAM].getValue()>0.5f){
            out = v>0.0f ? v : 0.0f;
        } else {
            out = tanh(v);
        }
        //        printf("%d, %f, %f\n",snark++,v,out);
        
        out *= params[POSTSCALE_PARAM].getValue();
        
        if(outputs[OUT_OUTPUT].isConnected()){
            outputs[OUT_OUTPUT].setChannels(1);
            outputs[OUT_OUTPUT].setVoltage(out);
        }
    }
    
};

struct ActFuncSwitch : CKSS {
    ActFuncSwitch() {
        shadow->hide();
    }
};


struct NeuronWidget : ModuleWidget {
#define KNOBSTYLE RoundBlackKnob
    NeuronWidget(Neuron *module){
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Neuron.svg")));
        
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        addParam(createParamCentered<KNOBSTYLE>(mm2px(W1_PARAM_POS), module, W1_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(IN1_INPUT_POS), module, IN1_INPUT));
        
        addParam(createParamCentered<KNOBSTYLE>(mm2px(W2_PARAM_POS), module, W2_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(IN2_INPUT_POS), module, IN2_INPUT));
        
        addParam(createParamCentered<KNOBSTYLE>(mm2px(W3_PARAM_POS), module, W3_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(IN3_INPUT_POS), module, IN3_INPUT));
        
        addParam(createParamCentered<KNOBSTYLE>(mm2px(W4_PARAM_POS), module, W4_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(IN4_INPUT_POS), module, IN4_INPUT));
        
        addParam(createParamCentered<KNOBSTYLE>(mm2px(W5_PARAM_POS), module, W5_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(IN5_INPUT_POS), module, IN5_INPUT));
        
        addParam(createParamCentered<KNOBSTYLE>(mm2px(PRESCALEMOD_PARAM_POS), module, PRESCALEMOD_PARAM));
        addParam(createParamCentered<KNOBSTYLE>(mm2px(PRESCALE_PARAM_POS), module, PRESCALE_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(PRESCALEMOD_INPUT_POS), module, PRESCALEMOD_INPUT));
        
        addParam(createParamCentered<KNOBSTYLE>(mm2px(BIAS_PARAM_POS), module, BIAS_PARAM));
        addParam(createParamCentered<ActFuncSwitch>(mm2px(ACTFUNC_PARAM_POS), module, ACTFUNC_PARAM));
        
        addParam(createParamCentered<KNOBSTYLE>(mm2px(POSTSCALE_PARAM_POS), module, POSTSCALE_PARAM));
        
        addOutput(createOutputCentered<PJ301MPort>(mm2px(OUT_OUTPUT_POS), module, OUT_OUTPUT));
        
    }
};

Model *modelNeuron = createModel<Neuron,NeuronWidget>("Neuron");

