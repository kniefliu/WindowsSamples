# Change Logs

## 1.0.0 2020-07-05 Feature alpha
- working time
	- 2020-07-10_20-00 to 2020-07-10_22-00
	- 2020-07-14_20-00 to 2020-07-14_22-00
	- 2020-07-15_09-00 to 2020-07-15_10-00
- Initial alpha
	- core module :
		- bitmap : gdi bitmap to support offscreen
	- bugs :
		- GDI:
			- Must use the hdc of the window to create memory hdc and memory bitmap.
			- Mustn't use memory hdc to create memory bitmap.

## 1.0.0 2020-07-05 Feature alpha
- working time
	- 2020-07-05_09-10 to 2020-07-04_19-50
- Initial alpha
	- core module :
		- color : refer to skia color
		- assert related macro : refer to skia assert.
		- control and paint manager : (1)only support on control; (2)only support simple WM_PAINT and WM_SIZE; 
		- canvas : (1)only support window hdc; (2)only support 'DrawColor'
	- utils module:
		- string and 'string key map' : (1)used by name of other class; (2)string key hash;
		

## 1.0.0 2020-07-04 Feature alpha
- working time
	- 2020-07-04_12-50 to 2020-07-04_21-30
- Initial alpha
	- core module : window, only showed, no more functions.

