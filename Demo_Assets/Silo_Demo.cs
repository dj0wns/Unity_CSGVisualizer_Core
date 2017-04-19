using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Parabox.CSG;
using System.Linq;

/**
 * Simple demo of CSG operations.
 */
public class Demo : MonoBehaviour
{
	GameObject composite, composite_normal,  temp;
	bool wireframe = false;

	Vector3 origin = new Vector3(0.0f, 0f, 0.0f);
	Vector3 regionSize = new Vector3(5f,5f,5f);

	List<GameObject> objects; //keep track of objects created
	List<GameObject> points; //keep track of points created
	
	public Material wireframeMaterial = null;
	public Material genericMaterial = null;
	public Material wireframeMaterial_red = null;
	public Material wireframeMaterial_blue = null;
	public Material wireframeMaterial_green = null;


	void Awake()
	{
		objects = new List<GameObject>();

		Silo_Test();
		
	}
	
	void Silo_Test(){
		SiloData sd = SiloReader.ReadFile("Assets/pb_CSG/Demo Assets/DATA/csg.fake");
	//	SiloReader.PrintStructure(sd);
		List<CSG_Tree> csgtree = SiloReader.GenerateTree(sd);
		for(int i = 0; i < csgtree.Count; i++){
			try{
				csgtree[i].render();

				//create new game object from result
  				composite= new GameObject();
  				composite.transform.position = origin;
				composite.AddComponent<MeshFilter>().sharedMesh = csgtree[i].getMesh();
	  			composite.AddComponent<MeshRenderer>().material = genericMaterial;
				composite.GetComponent<MeshRenderer>().material.color = sd.materials[sd.matlist[i]].color;
			
				objects.Add(composite);
			} catch(Exception e) {
				Debug.Log(e);
			}
			csgtree[i].remove_references();
		}

	}

}
