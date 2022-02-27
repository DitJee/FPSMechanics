# FPSMechanics

This documentation will cover all three gameplay mechanics: Dashing, Grav Gun, and Jetpack. The demo can be viewed from the project files and YouTube videos via these link: 

Google Drive: https://drive.google.com/drive/folders/1D9Jh5nxNgXfVwoBtVjs8SRxM_7eolvlJ?usp=sharing
  
  Dash: https://youtu.be/YC6Gau_2Em0 

  Grav Gun: https://youtu.be/GvKbuojtgiI 

  Jetpack: https://youtu.be/-KwbHFrB77Q 

## Dash
The dash is the first mechanic implemented in the test. This mechanic took approximately an hour of work to create and tune to the author personally liking. 
The dash implemented in the test is multi-direction which means the player can perform the dash in directions depending on controller input.
The parameters that the designer can change are dash speed and distance.

```c++
/** Dash Speed */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_Dash)
  float DashSpeed; 

/** Dash Distance */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_Dash)
  float DashDistance;
```

 The Functions of this mechanic can be summarized in the table below.
 
| Function Name        | Input           | Output  | Description |
| ------------- |:-------------:|:-------------: |:----- |
| `FVector` GetDashDirection()      | - | `FVector` DashDirection |Get the dash direction based on player input |
| `void` OnDash()    | -   |   -| Event that fired when the player perform the dash and set a delay to disable the ability to dash for a second |
| `void` Dash() | -     |   - | Update the player's location overtime until the distance travel is more than or equal to `DashDistance` |
| `void` ResetDash() | -     |    - | Give back the player's ability to dash |

## Grav Gun
The Grav Gun is probably the most fun mechanic to work on in this test since it is inspired by one of the most popular games, Half-life 2.
Essentially, this mechanic allows the player to pick up the object which is simulating physics and fire it. 
The design can tune two parameters: firing force and pick-up radius.

```c++
/** distance between held object and muzzle*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_GravGun)
  float HoldingRange = 300.0f;

/** distance allowed to pickup an object*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_GravGun)
  float PickUpRadius;

/** Impulse with fire an object*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_GravGun)
  float LaunchForce = 500.0f;

```

 The Functions of this mechanic can be summarized in the table below.
 
| Function Name        | Input           | Output  | Description |
| ------------- |:-------------:|:-------------: |:----- |
| `void` CheckIsHit()    | -   |   -| Perform a line tracing to check whether the hit object is simulating physic or not. Then, assign the object as `PickUpComponent`  |
| `void` OnPickUp() | -     |   - | Grab the `PickUpComponent` and attach it to `GrabLocation` |
| `void` OnDrop() | -     |    - | Release the `PickUpComponent` from the Gravity Gun|

## Jetpack
The last mechanic in the test is the Jetpack. The mechanic allows the player to launch himself in the air when the player is holding `spacebar`. 
The parameters that the design has control over are maximum fuel level, starting fuel level, thrust, air brake factor, refueling rate, fuel consumption rate, and time before refueling. 
```c++
/** Maximum fuel allowed*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
  float MaxFuel = 100.0f;

/** Starting fuel*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
  float Fuel = 100.0f;

/** Jetpack thrust*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
  float Thrust = 100.0f;

/** the decrease in movement velocity in percent*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
  float AirBreak = 0.75f;

/** The rate of which the fuel is refueled*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
  float RefuelingRate = 10.f;

/** The rate of fuel consumption*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
  float FuelConsumptionRate = 1.f;

/** The time in which the fuel will be refueled*/
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
  float RefuelDelay = 2.f;
  ```
  The Functions of this mechanic can be summarized in the table below.
  
| Function Name        | Input           | Output  | Description |
| ------------- |:-------------:|:-------------: |:----- |
| `void` SetIsRefuelable()    | -   |   -| Set refuelable flag to `true` which allows the fuel level to be increased  |
| `void` FallDown() | -     |   - | Deactivate the particle effect and SFX. Then, Set the character's gravity scale to `1` |
| `void` ConsumeFuel() | -     |    - | Calculate and decrease the fuel level overtime|
| `void` OnPressedJump() | -     |   - | Determine whether the character should perform a jetpack or a normal jump |
| `void` OnReleasedJump() | -     |    - | Stop the jumping or jetpack mechanic. Then, set a delay before the refueling is allowed|

## Widgets

To make a better user experience, the crosshair and fuel level widgets are created. The widget is composed inside UMG and bind its functionality inside C++.
These widgets can be access via `HUD` component.

### HUD
This component acts as a medium between the character and other widgets.
The variables inside the component can be summarized in the gist below.


```c++
public:
AFPSMechanicsHUD();

virtual void BeginPlay() override;

void DrawCrosshair(bool bFlag);

void SetFuelLevel(float fuelLevel);

UPROPERTY(EditAnywhere)
  TSubclassOf<UFPSMECHCrossHair> WBP_Crosshair;

UPROPERTY(EditAnywhere)
  TSubclassOf<UFPSMECHFuelTank> WBP_FuelTank;
private:

UUserWidget* W_Crosshair;

UUserWidget* W_FuelTank;
```
The Function of this HUD can be summarized in the table below.

| Function Name        | Input           | Output  | Description |
| ------------- |:-------------:|:-------------: |:----- |
| `void` DrawCrosshair()    | -   |   -| Set the flag to tell the crosshair to change its color  |
| `void` SetFuelLevel() | -     |   - | Pass the new fuel level to be updated in the fuel level widget |
| `virtual void` BeginPlay() `override`| -     |    - | Create widgets and add them to viewport|

### Crosshair
The main feature of this widget is the ability to change its color when the line tracing hits the object of interest as shown in the figure below.

![image](https://user-images.githubusercontent.com/56587469/155869674-2ad9be5c-2d32-4138-a7a8-81a41f7bc1ff.png)

The color of the crosshair is binded to a function inside the `Blueprint` which changes the color based on `bIsHit` boolean.

The variables inside the component can be summarized in the gist below.

```c++
protected:
virtual void NativeConstruct() override;

public:

UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bIsHit;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
  UImage* IconImage;
```
The Function of this widget can be summarized in the table below.

| Function Name        | Input           | Output  | Description |
| ------------- |:-------------:|:-------------: |:----- |
| `virtual void` NativeConstruct() `override`| -     |    - | Set `bIsHit` to `false`|

### Fuel level
This widget consists of a single `Progress Bar` which represents the fuel level inside the Jetpack as shown in the figure below.

![image](https://user-images.githubusercontent.com/56587469/155870193-6466fe69-44c1-43d6-82b6-9887e3df4c86.png)

The variables inside the component can be summarized in the gist below.

```c++
protected:
virtual void NativeConstruct() override;

public:
UPROPERTY(meta = (BindWidget))
  UProgressBar* FuelTank;

void SetFuelLevel(float fuelLevel);
```
The Function of this widget can be summarized in the table below.

| Function Name        | Input           | Output  | Description |
| ------------- |:-------------:|:-------------: |:----- |
| `virtual void` NativeConstruct() `override`| -     |    - | Set the fuel level to 100%|
| `void` SetFuelLevel(float fuelLevel)| -     |    - | Update the new fuel level|
