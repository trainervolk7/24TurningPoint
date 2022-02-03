#include "main.h"
#define clampPiston 'B'
float x=-2.0;

std::shared_ptr<ChassisController> drive =
	ChassisControllerBuilder()
		.withMotors( {-1,2,-3},{11,-12,13})
		// Green gearset, 4 in wheel diam, 11.5 im wheel track
		// 36 to 60 gear ratio
		.withDimensions({AbstractMotor::gearset::blue, (60.0/36.0)},{{4.125_in, 15_in}, imev5GreenTPR})
		.build();
		std::shared_ptr<AsyncMotionProfileController> profileController =
				  AsyncMotionProfileControllerBuilder()
				    .withLimits({
				      1.0, // Maximum linear velocity of the Chassis in m/s
				      2.0, // Maximum linear acceleration of the Chassis in m/s/s
				      10.0 // Maximum linear jerk of the Chassis in m/s/s/s
				    })
				    .withOutput(drive)
						.buildMotionProfileController();
		Controller controller;
		pros::ADIDigitalOut clamp (clampPiston);
		ControllerButton clampButton (ControllerDigital::R1);
		ControllerButton liftUpButton (ControllerDigital::L1);
		ControllerButton liftDownButton (ControllerDigital::L2);
		ControllerButton goalLiftUpButton (ControllerDigital::A); //change?
		ControllerButton goalLiftDownButton (ControllerDigital::X);//change?
		ControllerButton ringIntakeButton (ControllerDigital::Y);
		ControllerButton ringNonIntakeButton (ControllerDigital::B);
		bool isClampClosed = false;
		bool isRingOn = false;
		MotorGroup goalLift {-8,18};
		Motor ringMotor {14}; //change?
		MotorGroup lift {5,15};
		std::shared_ptr<AsyncPositionController<double, double>> goalLiftControl =
	AsyncPosControllerBuilder()
		.withMotor(goalLift)
		.build();
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {

}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	clamp.set_value(true);
	goalLift.setBrakeMode(AbstractMotor::brakeMode(2));
	goalLift.setGearing(AbstractMotor::gearset::red);
	goalLift.setEncoderUnits(AbstractMotor::encoderUnits::degrees);
	goalLift.tarePosition();

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	while (true) {
		drive->getModel() -> arcade(controller.getAnalog(ControllerAnalog::leftY), controller.getAnalog(ControllerAnalog::rightX));
		pros::delay(10);
		if (clampButton.isPressed())
		{
			if(isClampClosed){
			clamp.set_value(false);
			isClampClosed=false;
			pros::delay(200);
		}else{
			clamp.set_value(true);
			isClampClosed=true;
			pros::delay(200);
		}
		}

		if (liftUpButton.changedToPressed())
		{
			lift.moveVelocity(-100);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-500);
			}
		}
		else if(liftUpButton.changedToReleased())
		{
			lift.moveVoltage(0);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-500);
			}
		}
		else if(liftDownButton.changedToPressed())
		{
			lift.moveVelocity(900);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-500);
			}
		}
		else if (liftDownButton.changedToReleased())
		{
			lift.moveVoltage(0);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-500);
			}
		}

		if (goalLiftUpButton.isPressed())
		{
			//goalLift.getPosition()==0
			//goalLift.getTargetVelocity()==100
			//if(goalLift.getPosition()!=x){
			//goalLiftControl->setTarget(400);
			goalLift.moveAbsolute(-550, 200);
			pros::delay(20);
			//x=goalLift.getPosition();
			//pros::delay(20);
		//}else if(goalLift.getPosition()==x){
			//goalLiftControl->setTarget(0);
			//goalLift.moveVelocity(100);
			//goalLift.moveAbsolute(-100, 200);
			//pros::delay(20);
		//}
	}
	if (goalLiftDownButton.isPressed()){
		goalLift.moveAbsolute(550, 200);
		pros::delay(20);
	}

	if (ringIntakeButton.isPressed())
{
	if (isRingOn == false) {
		ringMotor.moveVelocity(300);
		isRingOn = true;
	} else {
		ringMotor.moveVelocity(0);
		isRingOn = false;
	}
	pros::delay(200);
}

if (ringNonIntakeButton.isPressed())
{
	if (isRingOn == false) {
		ringMotor.moveVelocity(-300);
		isRingOn = true;
	} else {
		ringMotor.moveVelocity(0);
		isRingOn = false;
	}
	pros::delay(200);
}

}
}
//TO do
//possibly change buttons and names for goal lift (search "change")
