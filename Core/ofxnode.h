//  Powiter
//
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 *Created by Alexandre GAUTHIER-FOICHAT on 6/1/2012.
 *contact: immarespond at gmail dot com
 *
 */



#ifndef OFXNODE_H
#define OFXNODE_H

#include "Core/node.h"
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QObject>
//ofx
#include "ofxhImageEffect.h"

//ours
class OFX::Host::ImageEffect::ClipInstance;

class OfxNode : public QObject,public Node,public OFX::Host::ImageEffect::Instance
{
    Q_OBJECT
    
    
    QMutex _lock;
    bool _firstTime;
    
public:
    OfxNode(OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
            OFX::Host::ImageEffect::Descriptor         &other,
            const std::string  &context);
    
    virtual ~OfxNode(){}
    
    virtual bool isInputNode();
    
    virtual bool isOutputNode();
    
    /*Returns the clips count minus the output clip*/
    virtual int maximumInputs();
    
    virtual int minimumInputs();
    
    virtual bool cacheData(){return false;}
    
    virtual const std::string className(){return getShortLabel();}
    
    virtual const std::string description(){return "";}
    
    virtual std::string setInputLabel(int inputNb);
    
    virtual bool isOpenFXNode() const {return true;}
    
    static ChannelSet ofxComponentsToPowiterChannels(const std::string& comp);
    
    virtual ChannelSet supportedComponents();
    
    virtual void _validate(bool){_firstTime = true;}
    
    virtual void engine(int y,int offset,int range,ChannelSet channels,Row* out);
    
    /// get default output fielding. This is passed into the clip prefs action
    /// and  might be mapped (if the host allows such a thing)
    virtual const std::string &getDefaultOutputFielding() const {
        static const std::string v(kOfxImageFieldNone);
        return v;
    }
    
    /// make a clip
    virtual OFX::Host::ImageEffect::ClipInstance* newClipInstance(OFX::Host::ImageEffect::Instance* plugin,
                                          OFX::Host::ImageEffect::ClipDescriptor* descriptor,
                                                                  int index);
    virtual OfxStatus vmessage(const char* type,
                               const char* id,
                               const char* format,
                               va_list args) {
        printf("%s %s ",type,id);
        vprintf(format,args);
        return kOfxStatOK;
    }

    //
    // live parameters
    //
    
    // The size of the current project in canonical coordinates.
    // The size of a project is a sub set of the kOfxImageEffectPropProjectExtent. For example a
    // project may be a PAL SD project, but only be a letter-box within that. The project size is
    // the size of this sub window.
    virtual void getProjectSize(double& xSize, double& ySize) const{
        xSize = _info->getDisplayWindow().w();
        ySize = _info->getDisplayWindow().h();
    }
    
    // The offset of the current project in canonical coordinates.
    // The offset is related to the kOfxImageEffectPropProjectSize and is the offset from the origin
    // of the project 'subwindow'. For example for a PAL SD project that is in letterbox form, the
    // project offset is the offset to the bottom left hand corner of the letter box. The project
    // offset is in canonical coordinates.
    virtual void getProjectOffset(double& xOffset, double& yOffset) const{
        xOffset = _info->x();
        yOffset = _info->y();
    }
    
    // The extent of the current project in canonical coordinates.
    // The extent is the size of the 'output' for the current project. See ProjectCoordinateSystems
    // for more infomation on the project extent. The extent is in canonical coordinates and only
    // returns the top right position, as the extent is always rooted at 0,0. For example a PAL SD
    // project would have an extent of 768, 576.
    virtual void getProjectExtent(double& xSize, double& ySize) const {
        xSize = _info->w();
        ySize = _info->h();
    }
    
    // The pixel aspect ratio of the current project
    virtual double getProjectPixelAspectRatio() const {
        return _info->getDisplayWindow().pixel_aspect();
    }
    
    // The duration of the effect
    // This contains the duration of the plug-in effect, in frames.
    virtual double getEffectDuration() const {return 1.0;}
    
    // For an instance, this is the frame rate of the project the effect is in.
    virtual double getFrameRate() const {return 25.0;}
    
    /// This is called whenever a param is changed by the plugin so that
    /// the recursive instanceChangedAction will be fed the correct frame
    virtual double getFrameRecursive() const {return 0.0;}
    
    /// This is called whenever a param is changed by the plugin so that
    /// the recursive instanceChangedAction will be fed the correct
    /// renderScale
    virtual void getRenderScaleRecursive(double &x, double &y) const{
        x = y = 1.0;
    }

    virtual OFX::Host::Param::Instance* newParam(const std::string& name, OFX::Host::Param::Descriptor& Descriptor);
    
    
    /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
    ///
    /// Client host code needs to implement this
    virtual OfxStatus editBegin(const std::string& name);
    
    /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
    ///
    /// Client host code needs to implement this
    virtual OfxStatus editEnd();
    
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for Progress::ProgressI
    
    /// Start doing progress.
    virtual void progressStart(const std::string &message);
    
    /// finish yer progress
    virtual void progressEnd();
    
    /// set the progress to some level of completion, returns
    /// false if you should abandon processing, true to continue
    virtual bool progressUpdate(double t);
    
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for TimeLine::TimeLineI
    
    /// get the current time on the timeline. This is not necessarily the same
    /// time as being passed to an action (eg render)
    virtual double timeLineGetTime();
    
    /// set the timeline to a specific time
    virtual void timeLineGotoTime(double t);
    
    /// get the first and last times available on the effect's timeline
    virtual void timeLineGetBounds(double &t1, double &t2);

public slots:
    void onInstanceChangedAction(const QString&);
};

#endif // OFXNODE_H
