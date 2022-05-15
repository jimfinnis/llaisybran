#include "plugin.hpp"

// number of in-param-out "channels", but I can't call it channels because that's
// the name polyphony uses.

static const int NUMPATHS = 4; 
struct OctaveLimiter : Module {
    enum ParamId {
        OCT1_PARAM,
              OCT2_PARAM,
              OCT3_PARAM,
              OCT4_PARAM,
              NOTE1_PARAM,
              NOTE2_PARAM,
              NOTE3_PARAM,
              NOTE4_PARAM,
              WIDTH1_PARAM,
              WIDTH2_PARAM,
              WIDTH3_PARAM,
              WIDTH4_PARAM,
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
    
    OctaveLimiter() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        for(int i=0;i<NUMPATHS;i++){
            configParam(OCT1_PARAM+i, -3.0f, 3.0f, 0.0f, "Octave of range start");
            configParam(NOTE1_PARAM+i, 0.0f, 12.0f, 0.0f, "Semitone within octave of range start");
            configParam(WIDTH1_PARAM+i, 1.0f, 3.0f, 1.0f, "Width of range in octaves");
            
            configInput(IN1_INPUT+i, "1V/octave pitch input");
            configOutput(OUT1_OUTPUT+i, "1V/octave pitch output");
            configBypass(IN1_INPUT+i, OUT1_OUTPUT+i);
        }
    }
    
    void process(const ProcessArgs& args) override {
        for(int i=0;i<NUMPATHS;i++){
            if(outputs[OUT1_OUTPUT+i].isConnected()){
                if(inputs[IN1_INPUT+i].isConnected()){
                    int channels = std::max(inputs[IN1_INPUT+i].getChannels(),1);
                    
                    int octParam = (int)(std::round(params[OCT1_PARAM+i].getValue()));
                    int noteParam = (int)(std::round(params[NOTE1_PARAM+i].getValue()));
                    int widthParam = (int)(std::round(params[WIDTH1_PARAM+i].getValue()));
                    
                    for(int c = 0; c<channels; c++){
                        float pitch = inputs[IN1_INPUT+i].getPolyVoltage(c);
                        // get note - it's 1V/oct
                        int note = (int)((pitch*12.0f)+0.001f);
                        
                        note += 1200; // ensure +ve
                        note -= noteParam; // remove start of range
                        note = note%(12*widthParam); // remove octave
                        note += octParam*12;         // add start of output octave
                        note += noteParam; // add back start of range
                        
                        pitch = ((float)note) / 12.0f;
                        outputs[OUT1_OUTPUT+i].setVoltage(pitch, c);
                    }
                    outputs[OUT1_OUTPUT+i].setChannels(channels);
                } else {
                    outputs[OUT1_OUTPUT+i].setVoltage(0);
                }
            }
        }
    }
};


struct OctaveLimiterWidget : ModuleWidget {
    
    static Knob *snap(OctaveLimiter *m, Knob *k){
        if(m){
            k->getParamQuantity()->smoothEnabled=false;
            k->getParamQuantity()->snapEnabled=true;
        }
        return k;
    }
    
#define KNOBSTYLE RoundBlackKnob
    
    OctaveLimiterWidget(OctaveLimiter* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/OctaveLimiter.svg")));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(37.754, 47.246)), module, OctaveLimiter::OCT1_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(53.834, 47.246)), module, OctaveLimiter::OCT2_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(69.915, 47.246)), module, OctaveLimiter::OCT3_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(85.996, 47.246)), module, OctaveLimiter::OCT4_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(37.754, 65.767)), module, OctaveLimiter::NOTE1_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(53.834, 65.767)), module, OctaveLimiter::NOTE2_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(69.915, 65.767)), module, OctaveLimiter::NOTE3_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(85.996, 65.767)), module, OctaveLimiter::NOTE4_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(37.754, 85.876)), module, OctaveLimiter::WIDTH1_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(53.834, 85.876)), module, OctaveLimiter::WIDTH2_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(69.915, 85.876)), module, OctaveLimiter::WIDTH3_PARAM)));
        addParam(snap(module,createParamCentered<KNOBSTYLE>(mm2px(Vec(85.996, 85.876)), module, OctaveLimiter::WIDTH4_PARAM)));
        
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.754, 29.681)), module, OctaveLimiter::IN1_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.834, 29.681)), module, OctaveLimiter::IN2_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(69.915, 29.681)), module, OctaveLimiter::IN3_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.996, 29.681)), module, OctaveLimiter::IN4_INPUT));
        
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.754, 113)), module, OctaveLimiter::OUT1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.834, 113)), module, OctaveLimiter::OUT2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.915, 113)), module, OctaveLimiter::OUT3_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.996, 113)), module, OctaveLimiter::OUT4_OUTPUT));
    }
};


Model* modelOctaveLimiter = createModel<OctaveLimiter, OctaveLimiterWidget>("OctaveLimiter");