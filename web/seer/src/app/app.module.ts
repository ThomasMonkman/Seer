import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { NgModule } from '@angular/core';

import { CommonModule } from '@angular/common';

import 'hammerjs';

import { LocalStorageModule, LocalStorageService } from 'angular-2-local-storage';
import { AppComponent } from './app.component';
import { SeerConnectionService } from './services/seer-connection/seer-connection.service';
import { DashboardComponent } from './views/dashboard/dashboard.component';
import { SeerMaterialModule } from './modules/seer-material-module/seer-material-module.module';
import { ConnectionComponent } from './views/dashboard/connection/connection.component';
import { FormsModule, ReactiveFormsModule } from "@angular/forms";
import { NewConnectionComponent } from './views/dashboard/new-connection/new-connection.component';
import { ConnectionHistoryService } from 'app/services/connection-history/connection-history.service';

@NgModule({
  declarations: [
    AppComponent,
    DashboardComponent,
    ConnectionComponent,
    NewConnectionComponent
  ],
  imports: [
    BrowserModule,
    BrowserAnimationsModule,
    CommonModule,
    FormsModule,
    SeerMaterialModule,
    ReactiveFormsModule,
    LocalStorageModule.withConfig({
      prefix: 'seer',
      storageType: 'localStorage'
    })
  ],
  providers: [
    LocalStorageService,
    SeerConnectionService,
    ConnectionHistoryService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
