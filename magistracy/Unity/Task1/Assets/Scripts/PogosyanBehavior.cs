using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PogosyanBehavior : MonoBehaviour
{
    Renderer renderer;
    // Start is called before the first frame update
    void Start()
    {
        renderer = GetComponent<Renderer>();
        renderer.enabled = false;
        StartCoroutine("HideUnhide");
    }

    IEnumerator HideUnhide()
    {
        yield return (new WaitForSeconds(6));
        renderer.enabled = true;
    }
        // Update is called once per frame
        void Update()
    {
        
    }
}
