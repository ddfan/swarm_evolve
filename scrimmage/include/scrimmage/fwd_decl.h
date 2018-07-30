#ifndef FWD_DECL_H
#define FWD_DECL_H

#include <memory>
#include <unordered_map>

namespace GeographicLib {
class LocalCartesian;
using LocalCartesianPtr = std::shared_ptr<LocalCartesian>;
}

namespace scrimmage {
class Autonomy;
using AutonomyPtr = std::shared_ptr<Autonomy>;

class State;
using StatePtr = std::shared_ptr<State>;

class Random;
using RandomPtr = std::shared_ptr<Random>;

class Sensable;
using SensablePtr = std::shared_ptr<Sensable>;

class Contact;
using ContactMap = std::unordered_map<int, Contact>;
using ContactMapPtr = std::shared_ptr<ContactMap>;

class Entity;
using EntityPtr = std::shared_ptr<Entity>;

class Network;
using NetworkPtr = std::shared_ptr<Network>;

class Publisher;
using PublisherPtr = std::shared_ptr<Publisher>;

class Subscriber;
using SubscriberPtr = std::shared_ptr<Subscriber>;

class MessageBase;
using MessageBasePtr = std::shared_ptr<MessageBase>;

class RTree;
using RTreePtr = std::shared_ptr<RTree>;

class Plugin;
using PluginPtr = std::shared_ptr<Plugin>;

class PluginManager;
using PluginManagerPtr = std::shared_ptr<PluginManager>;

class MissionParse;
using MissionParsePtr = std::shared_ptr<MissionParse>;

class MotionModel;
using MotionModelPtr = std::shared_ptr<MotionModel>;

class Controller;
using ControllerPtr = std::shared_ptr<Controller>;

class Sensor;
using SensorPtr = std::shared_ptr<Sensor>;

class Log;

class Interface;
using InterfacePtr = std::shared_ptr<Interface>;

class EntityInteraction;
using EntityInteractionPtr = std::shared_ptr<EntityInteraction>;

class Metrics;
using MetricsPtr = std::shared_ptr<Metrics>;

class CameraInterface;
}

#endif // FWD_DECL_H
