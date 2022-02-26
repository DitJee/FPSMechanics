# FPSMechanics

This documentation will cover all three gameplay mechanics: Dashing, Grav Gun, and Jetpack. The demo can be viewed from the project files and YouTube video via this link: https://www.youtube.com/channel/UCfIS1JjgfnvLFcME6ld-9wg.
## Dash
The dash is the first mechanic implemented in the test. This mechanic took approximately an hour of work to create and tune to the author personally liking. 
The dash implemented in the test is octa-direction which means the player can perform the dash in 8 directions: north, east, south, west, northeast, southeast, southwest, and northwest as shown in figure below.
The parameters that the designer can change are dash speed and distance.

```c++
/** Dash Speed */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_Dash)
  float DashSpeed; 

/** Dash Distance */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_Dash)
  float DashDistance;
```

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
