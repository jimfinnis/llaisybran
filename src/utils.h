/**
 * @file utils.h
 * @brief  Brief description of file.
 *
 */

#ifndef __UTILS_H
#define __UTILS_H


inline Knob *snap(Module *m, Knob *k){
    if(m){
        k->getParamQuantity()->smoothEnabled=false;
        k->getParamQuantity()->snapEnabled=true;
    }
    return k;
}

#endif /* __UTILS_H */
