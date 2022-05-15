#include "plugin.hpp"

// number of in-param-out "channels", but I can't call it channels because that's
// the name polyphony uses.

static const int NUMPATHS = 4; 
struct CVFold : Module {
    enum ParamId {
        OCT1_PARAM,
              OCT2_PARAM,
              OCT3_PARAM,
              OCT4_PARAM,
              PARAMS_LEN
          };
    enum InputId {
        IN1_INPUT,
              IN2_INPUT,
              IN3_INPUT,
              IN4_INPUT,
              INPUTS_LEN
          };
    enum OutputId {
        OUT1_OUTPUT,
              OUT2_OUTPUT,
              OUT3_OUTPUT,
              OUT4_OUTPUT,
              OUTPUTS_LEN
          };
    enum LightId {
        LIGHTS_LEN
          };
    
    CVFold() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        for(int i=0;i<NUMPATHS;i++){
            configParam(OCT1_PARAM+i, -3.0f, 3.0f, 0.0f, "Octave");
            configInput(IN1_INPUT+i, "1V/octave pitch input");
            configOutput(OUT1_OUTPUT+i, "1V/octave pitch output");
        }
    }
    
    void process(const ProcessArgs& args) override {
        static int prevn=-1;
        for(int i=0;i<NUMPATHS;i++){
            if(outputs[OUT1_OUTPUT+i].isConnected()){
                float pitch;
                if(inputs[IN1_INPUT+i].isConnected()){
                    pitch = inputs[IN1_INPUT+i].getVoltage();
                    float octF = params[OCT1_PARAM+i].getValue();
                    int oct = (int)(std::round(octF));
                    // 1V/oct
                    int note = (int)((pitch*12.0f)+0.001f);
                    if(note!=prevn){
                        printf("%d %d\n",note,oct);
                        prevn=note;
                    }
                    
                    note = note%12; // remove octave
                    note += oct*12;
                    
                    pitch = ((float)note) / 12.0f;
                } else {
                    pitch = 0;
                }
                outputs[OUT1_OUTPUT+i].setVoltage(pitch);
            }
        }
    }
};


struct CVFoldWidget : ModuleWidget {
    
    static Knob *snap(Knob *k){
        k->getParamQuantity()->smoothEnabled=false;
        k->getParamQuantity()->snapEnabled=true;
        return k;
    }
    
    CVFoldWidget(CVFold* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/CVFold.svg")));
        
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        addParam(snap(createParamCentered<RoundBlackKnob>(mm2px(Vec(37.754, 62.063)), module, CVFold::OCT1_PARAM)));
        addParam(snap(createParamCentered<RoundBlackKnob>(mm2px(Vec(53.834, 62.063)), module, CVFold::OCT2_PARAM)));
        addParam(snap(createParamCentered<RoundBlackKnob>(mm2px(Vec(69.915, 62.063)), module, CVFold::OCT3_PARAM)));
        addParam(snap(createParamCentered<RoundBlackKnob>(mm2px(Vec(85.996, 62.063)), module, CVFold::OCT4_PARAM)));
        
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.754, 29.681)), module, CVFold::IN1_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.834, 29.681)), module, CVFold::IN2_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(69.915, 29.681)), module, CVFold::IN3_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.996, 29.681)), module, CVFold::IN4_INPUT));
        
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.754, 98.25)), module, CVFold::OUT1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.834, 98.25)), module, CVFold::OUT2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.915, 98.25)), module, CVFold::OUT3_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.996, 98.25)), module, CVFold::OUT4_OUTPUT));
        
        
    }
};


Model* modelCVFold = createModel<CVFold, CVFoldWidget>("CVFold");