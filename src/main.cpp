#include "main.h"
#include <bits/specfun.h>
#define clampPiston 'B'
#define flipPiston 'E'
float x=-2.0;

std::shared_ptr<ChassisController> drive =
	ChassisControllerBuilder()
		.withMotors( {-11,12,-13},{1,-2,3})
		// Green gearset, 4 in wheel diam, 11.5 im wheel track
		// 36 to 60 gear ratio
		.withDimensions({AbstractMotor::gearset::blue, (60.0/36.0)},{{4.125_in, 15_in}, imev5GreenTPR})
		.build();
		std::shared_ptr<AsyncMotionProfileController> profileController =
				  AsyncMotionProfileControllerBuilder()
				    .withLimits({
				      1.0, // Maximum linear velocity of the Chassis in m/s
				      1.0, // Maximum linear acceleration of the Chassis in m/s/s
				      10.0 // Maximum linear jerk of the Chassis in m/s/s/s
				    })
				    .withOutput(drive)
						.buildMotionProfileController();
		Controller controller;
		pros::ADIDigitalOut clamp (clampPiston);
		pros::ADIDigitalOut flip (flipPiston);
		ControllerButton clampButton (ControllerDigital::R1);
		ControllerButton liftUpButton (ControllerDigital::L2);
		ControllerButton liftDownButton (ControllerDigital::L1);
		ControllerButton goalLiftDownButton (ControllerDigital::A); //change?
		ControllerButton goalLiftUpButton (ControllerDigital::X);//change?
		ControllerButton ringIntakeButton (ControllerDigital::up);
		ControllerButton ringNonIntakeButton (ControllerDigital::down);
		bool isClampClosed = false;
		bool isRingOn = false;
		MotorGroup goalLift {8,-18};
		Motor ringMotor {-6}; //change?
		MotorGroup lift {5,-15};
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

//LVGL Code
lv_obj_t * createBtn(lv_obj_t * parent, lv_coord_t x, lv_coord_t y, lv_coord_t width, lv_coord_t height,
    int id, const char * title)
{
    lv_obj_t * btn = lv_btn_create(parent, NULL);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, width, height);
    lv_obj_set_free_num(btn, id);

    lv_obj_t * label = lv_label_create(btn, NULL);
    lv_label_set_text(label, title);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

    return btn;
}







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
	flip.set_value(false);
	goalLift.setBrakeMode(AbstractMotor::brakeMode(2));
	goalLift.setGearing(AbstractMotor::gearset::red);
	goalLift.setEncoderUnits(AbstractMotor::encoderUnits::degrees);
	goalLift.tarePosition();
	flip.set_value(true);


//LVGL Code
//Width: 480 , height: 230 plus some

//createBtn(lv_obj_t * parent, lv_coord_t x, lv_coord_t y, lv_coord_t width, lv_coord_t height, int id, const char * title)

/*lv_style_copy(&style_btn, &lv_style_plain);
    style_btn.body.main_color = LV_COLOR_MAKE(255, 0, 0);
    style_btn.body.grad_color = LV_COLOR_MAKE(0, 0, 255);
    style_btn.body.radius = 0;
    style_btn.text.color = LV_COLOR_MAKE(255, 255, 255);

		lv_style_copy(&style_btnREL, &lv_style_plain);


		    style_btnREL.body.main_color = LV_COLOR_MAKE(150, 0, 0);
		    style_btnREL.body.grad_color = LV_COLOR_MAKE(0, 0, 150);
		    style_btnREL.body.radius = 0;
		    style_btnREL.text.color = LV_COLOR_MAKE(255, 255, 255);
		lv_obj_t * button = createBtn(lv_scr_act(),50 ,50,100,50 ,33 , "test");
		lv_btn_set_style(button, LV_BTN_STYLE_REL, &style_btnREL); //set the relesed style
		    lv_btn_set_style(button, LV_BTN_STYLE_PR, &style_btn); //set the pressed style
*/
	lv_obj_t * button = createBtn(lv_scr_act(),50 ,50,100,50 ,33 , "test");




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
void autonomous() {
	flip.set_value(false);
	pros::delay(1000);
	flip.set_value(true);
	profileController->generatePath(
	{{0_in, 0_in, 0_deg}, {16_in, 0_in, 0_deg}}, "A");
	goalLiftControl->setTarget(400);
	profileController->setTarget("A");
	profileController->waitUntilSettled();
	goalLiftControl->setTarget(0);
	pros::delay(2500);
	ringMotor.moveVelocity(200);
	pros::delay(2500);
	ringMotor.moveVelocity(0);
	profileController->setTarget("A",true);
	profileController->waitUntilSettled();


/*
new autonomous
24 bot starting on right
	-goes for the middle yellow goal first (since less people go for it)
	-comes back put middle goal on platform
	-remove mobile goal from line and place rings



*/



}
//flip.set_value(true);
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
				lift.moveVoltage(-700);
			}
		}
		else if(liftUpButton.changedToReleased())
		{
			lift.moveVoltage(0);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(-700);
			}
		}
		else if(liftDownButton.changedToPressed())
		{
			lift.moveVelocity(900);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(900);
			}
		}
		else if (liftDownButton.changedToReleased())
		{
			lift.moveVoltage(0);
			if (liftUpButton.isPressed()&&liftDownButton.isPressed())
			{
				lift.moveVoltage(900);
			}
		}

		if (goalLiftUpButton.changedToPressed())
		{
			goalLift.moveVelocity(100);
			//pros::delay(20);

			//goalLift.getPosition()==0
			//goalLift.getTargetVelocity()==100
			//if(goalLift.getPosition()!=x){
			//goalLiftControl->setTarget(400);
			//goalLift.moveAbsolute(-300, 400);
			//pros::delay(20);
			//x=goalLift.getPosition();
			//pros::delay(20);
		//}else if(goalLift.getPosition()==x){
			//goalLiftControl->setTarget(0);
			//goalLift.moveVelocity(100);
			//goalLift.moveAbsolute(-100, 200);
			//pros::delay(20);
		//}
	}
	else if (goalLiftUpButton.changedToReleased()){
		goalLift.moveVelocity(0);
		//pros::delay(20);
	}

	if (goalLiftDownButton.changedToPressed()){
		goalLift.moveVelocity(-100);
		//pros::delay(20);
	}
	else if (goalLiftDownButton.changedToReleased()){
		goalLift.moveVelocity(0);
		//pros::delay(20);
	}

	if (ringIntakeButton.isPressed())
{
	if (isRingOn == false) {
		ringMotor.moveVelocity(200);
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
		ringMotor.moveVelocity(-200);
		isRingOn = true;
	} else {
		ringMotor.moveVelocity(0);
		isRingOn = false;
	}
	pros::delay(200);
}

}
}


//programming skills
/*
24 starts on left side
-grabs mobile goal on seesaw with
*/
