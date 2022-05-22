using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FollowTarget : MonoBehaviour
{
    private Vector3 offset;

    [SerializeField] private Transform Target;
    
    // Start is called before the first frame update
    void Start()
    {
        offset = transform.position - Target.position;
    }

    // Update is called once per frame
    void Update()
    {
        transform.position = Target.position + offset;
        transform.rotation = Target.rotation;
    }
}

