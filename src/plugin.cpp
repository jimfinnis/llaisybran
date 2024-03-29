#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
    pluginInstance = p;
    
    // Add modules here
    p->addModel(modelOctaveLimiter);
    p->addModel(modelPolySelect);
    p->addModel(modelChord);
    p->addModel(modelChord2);
    p->addModel(modelNeuron);
    
    // Any other plugin initialization may go here.
    // As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
