#ifndef ANIMEVAL_H_
#define ANIMEVAL_H_

#include <assimp/scene.h>
#include <vector>
#include <map>

struct tuple {
	unsigned int i[3];

	tuple(unsigned int i0, unsigned int i1, unsigned int i2) { i[0] = i0; i[1] = i1; i[2] = i2; }
	unsigned int get(int index) { return i[index]; }
	void set(int index, unsigned int val) { i[index] = val; }
};

/** Calculates transformations for a given timestamp from a set of animation tracks. Not directly useful,
* better use the AnimPlayer class.
*/
class AnimEvaluator
{
public:
	/** Constructor on a given animation. The animation is fixed throughout the lifetime of
	* the object.
	* @param pAnim The animation to calculate poses for. Ownership of the animation object stays
	*   at the caller, the evaluator just keeps a reference to it as long as it persists.
	*/
	AnimEvaluator(const aiAnimation* pAnim);

	/** Evaluates the animation tracks for a given time stamp. The calculated pose can be retrieved as a
	* array of transformation matrices afterwards by calling GetTransformations().
	* @param pTime The time for which you want to evaluate the animation, in seconds. Will be mapped into the animation cycle, so
	*   it can be an arbitrary value. Best use with ever-increasing time stamps.
	*/
	void Evaluate(double pTime);

	/** Returns the transform matrices calculated at the last Evaluate() call. The array matches the mChannels array of
	* the aiAnimation. */
	const std::vector<aiMatrix4x4>& GetTransformations() const { return mTransforms; }

protected:
	/** The animation we're working on */
	const aiAnimation* mAnim;

	/** At which frame the last evaluation happened for each channel.
	* Useful to quickly find the corresponding frame for slightly increased time stamps
	*/
	double mLastTime;
	std::vector<tuple> mLastPositions;

	/** The array to store the transformations results of the evaluation */
	std::vector<aiMatrix4x4> mTransforms;
};
#endif