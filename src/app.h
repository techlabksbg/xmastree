/**
 * @file app.h
 * @author Ivo Blöchliger (ivo.bloechliger@ksbg.ch)
 * @brief 
 * @version 0.1
 * @date 2021-11-15
 * 
 * 
 */

#pragma once





class App {
    public:
    App();
    /**
     * @brief Returns the name of the App, used as Button-Text on the Webinterface
     * 
     * @return const char* Name of the app
     */
    virtual const char* buttonName() = 0; 
    /**
     * @brief Implement this to and return true, if your loop should be called as fast as possible
     * 
     * @return true Loop fast
     * @return false Loop at default speed (25fps)
     */
    virtual bool loopFast() { return false; }
    /**
     * @brief Set the Good Params object
     * 
     * @return true This app will be autoplayed
     * @return false This app can only be started manually via Webinterface
     */
    virtual bool setGoodParams() { return false; }
    /**
     * @brief Makes the animation. Do not forget to call params.pixels->Show() at the end!
     */
    virtual void loop() = 0;
    /**
     * @brief Gives the time seconds since the start of the app
     * 
     * @return float Time in seconds
     */
    float secs();
    /**
     * @brief Called when the app switches and loop will not be called anymore.
     * Clean up memory and open file handlers.
     * Open/Reallocate as necessary in the loop task
     */
    virtual void stop() {}
private:
    unsigned int start=0;
    unsigned int last=0;

};
