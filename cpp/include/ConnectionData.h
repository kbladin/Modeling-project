/** Structs **/

// A connection associates two masses
// and specifies the distance between them at rest.
struct Connection{
	float mass1;
	float mass2;
	float length;
};

// Adds spring and damper properties to a connection.
// Specifies the spring constant (k) and damper constant (b).
struct SpringDamper{
	float k;
	float b;
};

// Adds friction properties to a connection.
// Specifies the static and dynamic friction constants.
struct Friction{
	float staticFrictionConstant;
	float dynamicFrictionConstant;
};