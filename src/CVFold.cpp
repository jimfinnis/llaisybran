#include "plugin.hpp"


struct CVFold : Module {
    enum ParamId {
        OCT_PARAM,
              PARAMS_LEN
          };
    enum InputId {
        IN_INPUT,
              INPUTS_LEN
          };
    enum OutputId {
        OUT_OUTPUT,
              OUTPUTS_LEN
          };
    enum LightId {
        LIGHTS_LEN
          };
    
    CVFold() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(OCT_PARAM, 0.f, 1.f, 0.f, "");
        configInput(IN_INPUT, "");
        configOutput(OUT_OUTPUT, "");
    }
    
    void process(const ProcessArgs& args) override {
        static int prevn=-1;
        float pitch = inputs[IN_INPUT].getVoltage();
        float octF = params[OCT_PARAM].getValue();
        int oct = (int)(octF*5);
        // 1V/oct
        int note = (int)((pitch*12.0f)+0.001f);
        if(note!=prevn){
            printf("%d %d\n",note,oct);
            prevn=note;
        }
        
        note = note%12; // remove octave
        note += (oct-3)*12;
        
        pitch = ((float)note) / 12.0f;

        
        outputs[OUT_OUTPUT].setVoltage(pitch);
    }
};


struct CVFoldWidget : ModuleWidget {
    CVFoldWidget(CVFold* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/CVFold.svg")));
        
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(36.906, 62.063)), module, CVFold::OCT_PARAM));
        
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.375, 29.681)), module, CVFold::IN_INPUT));
        
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.226, 98.25)), module, CVFold::OUT_OUTPUT));
    }
};


Model* modelCVFold = createModel<CVFold, CVFoldWidget>("CVFold");