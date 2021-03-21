/*************************************************************************/
/*  ewbik_shadow_bone_3d.cpp                                             */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "ewbik_shadow_bone_3d.h"

void IKBone::set_bone_id(BoneId p_bone_id, Skeleton3D *p_skeleton) {
	for_bone = p_bone_id;
}

BoneId IKBone::get_bone_id() const {
	return for_bone;
}

void IKBone::set_parent(const Ref<IKBone> &p_parent) {
	parent = p_parent;
	if (parent.is_valid()) {
		parent->children.push_back(this);
		xform.set_parent(&parent->xform);
	}
}

Ref<IKBone> IKBone::get_parent() const {
	return parent;
}

void IKBone::set_effector(const Ref<IKEffector> &p_effector) {
	effector = p_effector;
}

Ref<IKEffector> IKBone::get_effector() const {
	return effector;
}

void IKBone::set_transform(const Transform &p_transform) {
	xform.set_transform(p_transform);
}

Transform IKBone::get_transform() const {
	return xform.get_transform();
}

void IKBone::rotate(const Quat &p_rot) {
	Transform rot_xform = Transform(Basis(p_rot), Vector3());
	set_transform(rot_xform * get_transform());
}

void IKBone::set_global_transform(const Transform &p_transform) {
	xform.set_global_transform(p_transform);
}

Transform IKBone::get_global_transform() const {
	return xform.get_global_transform();
}

void IKBone::set_initial_transform(Skeleton3D *p_skeleton) {
	set_transform(p_skeleton->get_bone_pose(get_bone_id()));
	if (is_effector())
		effector->update_goal_transform(p_skeleton);
}

void IKBone::create_effector() {
	effector = Ref<IKEffector>(memnew(IKEffector(this)));
}

bool IKBone::is_effector() const {
	return effector.is_valid();
}

Vector<BoneId> IKBone::get_children_with_effector_descendants(Skeleton3D *p_skeleton, const HashMap<BoneId, Ref<IKBone>> &p_map) const {
	Vector<BoneId> children_with_effector;
	Vector<BoneId> children = p_skeleton->get_bone_children(for_bone);
	for (int32_t child_i = 0; child_i < children.size(); child_i++) {
		BoneId child_bone = children[child_i];
		if (IKBone::has_effector_descendant(child_bone, p_skeleton, p_map)) {
			children_with_effector.push_back(child_bone);
		}
	}
	return children_with_effector;
}

bool IKBone::has_effector_descendant(BoneId p_bone, Skeleton3D *p_skeleton, const HashMap<BoneId, Ref<IKBone>> &p_map) {
	if (p_map.has(p_bone) && p_map[p_bone]->is_effector()) {
		return true;
	} else {
		bool result = false;
		Vector<BoneId> children = p_skeleton->get_bone_children(p_bone);
		for (int32_t child_i = 0; child_i < children.size(); child_i++) {
			BoneId child_bone = children[child_i];
			if (IKBone::has_effector_descendant(child_bone, p_skeleton, p_map)) {
				result = true;
				break;
			}
		}
		return result;
	}
}

void IKBone::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_effector"), &IKBone::get_effector);
	ClassDB::bind_method(D_METHOD("set_effector", "effector"), &IKBone::set_effector);
	ClassDB::bind_method(D_METHOD("is_pinned"), &IKBone::is_effector);
}

IKBone::IKBone(BoneId p_bone, const Ref<IKBone> &p_parent) {
	for_bone = p_bone;
	set_parent(p_parent);
}

IKBone::IKBone(String p_bone, Skeleton3D *p_skeleton, const Ref<IKBone> &p_parent) {
	for_bone = p_skeleton->find_bone(p_bone);
	set_parent(p_parent);
}