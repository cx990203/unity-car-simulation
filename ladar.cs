using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ladar : MonoBehaviour
{
    [SerializeField] private Transform Ladar;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    const int maxDis = 100;
    // Update is called once per frame
    void Update()
    {
        Ladar.Rotate(new Vector3(0, 1, 0));
        Ray ray = new Ray(Ladar.position, Ladar.forward * maxDis);
        RaycastHit hit;
        if (Physics.Raycast(ray, out hit, maxDis))
        {
            print(hit.distance);
            Debug.DrawLine(Ladar.position, hit.point, Color.red);
        }
        else
        {
            Debug.DrawLine(Ladar.position, Ladar.position + Ladar.forward * maxDis, Color.red);
        }
    }
}
