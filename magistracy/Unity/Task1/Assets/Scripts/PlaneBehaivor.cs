using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlaneBehaivor : MonoBehaviour
{
    public Vector3 direction;
    public float speed;

    // Start is called before the first frame update
    void Start()
    {
        direction = new Vector3(0.0f, 1.0f, 0.5f);
        speed = 0.03f;
    }

    // Update is called once per frame
    void FixedUpdate() //привязан к реальному таймеру, а не fps устройства
    {
        //1 method
        //transform.Translate(direction * Time.deltaTime);

        //2 method
        transform.Translate(direction * speed);
    }
}
