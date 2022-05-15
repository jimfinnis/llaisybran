#include "plugin.hpp"

struct PolySelect : Module {
    enum ParamId {
        MUL_PARAM,
              PARAMS_LEN
          };
    enum InputId {
        POLY_INPUT,
              SELECT_INPUT,
              INPUTS_LEN
          };
    enum OutputId {
        CV_OUTPUT,
              OUTPUTS_LEN
          };
    enum LightId {
        LIGHTS_LEN
          };
    
    PolySelect() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(MUL_PARAM, 0.0f, 20.0f, 1.0f, "Multiplier applied to select input");
        configInput(SELECT_INPUT, "Select integer input");
        configInput(POLY_INPUT, "Polyphonic CV input");
        configOutput(CV_OUTPUT, "CV output");
    }
    
    void process(const ProcessArgs& args) override {
        if(outputs[CV_OUTPUT].isConnected()){
            if(inputs[POLY_INPUT].isConnected()){
                int channels = std::max(inputs[POLY_INPUT].getChannels(),1);
                float mul = params[MUL_PARAM].getValue();
                float sel = inputs[SELECT_INPUT].getVoltage();
                
                int select = (int)std::round(sel*mul);
                if(select<0)select=0;
                select = select%channels;
                
                float v = inputs[POLY_INPUT].getPolyVoltage(select);
                outputs[CV_OUTPUT].setVoltage(v);
            } else {
                outputs[CV_OUTPUT].setVoltage(0);
            }
        }
    }
};


struct PolySelectWidget : ModuleWidget {
    
    static Knob *snap(PolySelect *m, Knob *k){
        if(m){
            k->getParamQuantity()->smoothEnabled=false;
            k->getParamQuantity()->snapEnabled=true;
        }
        return k;
    }
    
#define KNOBSTYLE RoundBlackKnob
    
    PolySelectWidget(PolySelect* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/PolySelect.svg")));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(39.9, 73.1)), module, PolySelect::MUL_PARAM)));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.9, 22.7)), module, PolySelect::POLY_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.9, 54.15)), module, PolySelect::SELECT_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.9, 113.0)), module, PolySelect::CV_OUTPUT));
    }
};


Model* modelPolySelect = createModel<PolySelect, PolySelectWidget>("PolySelect");