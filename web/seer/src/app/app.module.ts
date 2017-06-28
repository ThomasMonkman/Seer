import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { CommonModule } from '@angular/common';

import { AppComponent } from './app.component';
import { SeerConnectionService } from "app/services/seer-connection/seer-connection.service";
import { DashboardComponent } from './dashboard/dashboard.component';

@NgModule({
  declarations: [
    AppComponent,
    DashboardComponent
  ],
  imports: [
    BrowserModule,
    CommonModule
  ],
  providers: [
    SeerConnectionService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
