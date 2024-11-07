#include <iostream>

#include <flowcpp/flow.h>

template<typename PayloadType, typename ActionType>
struct ActionWithPayload {
  flow::any payload() const { return _payload; }
  flow::any type() const { return _type; }
  flow::any meta() const { return _meta; }
  bool error() const { return _error; }

  PayloadType _payload;
  ActionType _type;
  flow::any _meta;
  bool _error = false;
};

// AppStore model
enum class ScreenType {
  Setup,
  Live,
  Record,
  Other
};

enum class ScreenActionType {
  Transit
};

struct ScreenAction : ActionWithPayload<ScreenType, ScreenActionType> {
  static ScreenAction Transit(ScreenType to) {
    return ScreenAction{ScreenActionType::Transit, to};
  }

  ScreenAction(ScreenActionType type, const ScreenType to) {
    this->_payload = to;
    this->_type = type;
  }
};

auto screen_state_reducer = [](ScreenType state, flow::action action) {
  auto type = action.type().as<ScreenActionType>();
  switch (type) {
    case ScreenActionType::Transit:
      state = action.payload().as<ScreenType>();
      break;
  }
  return state;
};

struct PatientInfo {
  std::string name;
  std::string last_name;
};

enum PatientInfoActionType {
  Set
};

struct PatientInfoAction : ActionWithPayload<PatientInfo, PatientInfoActionType> {
  static PatientInfoAction Set(const PatientInfo& patient_info) {
    return PatientInfoAction{PatientInfoActionType::Set, patient_info};
  }
private:
  PatientInfoAction(PatientInfoActionType type, const PatientInfo& patient_info) {
    this->_payload = patient_info;
    this->_type = type;
  }
};

auto patient_info_reducer = [](const PatientInfo&, flow::action action) {
  auto type = action.type().as<PatientInfoActionType>();
  switch (type) {
    case PatientInfoActionType::Set:
      return action.payload().as<PatientInfo>();
  }

  return PatientInfo{"unknown", "unknown"};
};

struct AppState {
  flow::basic_store<ScreenType> screen = flow::create_store<ScreenType>(screen_state_reducer, ScreenType::Setup);
  flow::basic_store<PatientInfo> patientInfo = flow::create_store<PatientInfo>(patient_info_reducer, {});
};

int main() {
  // load the state at very beggining of the app
  AppState state;

  // in main ui screen
  auto screenSubscription = state.screen.subscribe(
    [](ScreenType state) {
      std::cout << "Navigate to: " << static_cast<int>(state) << std::endl;
    });

  // in patient info ui screen
  auto patientInfoSubscription = state.patientInfo.subscribe([](PatientInfo patient) {
      std::cout << "Load patient: " << patient.name << " " << patient.last_name << std::endl;
    });

  state.screen.dispatch(ScreenAction::Transit(ScreenType::Live));
  state.patientInfo.dispatch(PatientInfoAction::Set({"naum", "puroski"}));

  return 0;
}
